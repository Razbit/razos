/* This file is a part of the RazOS project
 *
 * heap.c -- heap allocation, freeing etc
 *
 * Razbit 2014 */

/* Dec. 26, 2014: Heap allocation and freeing work, heap expands and
 * contracts as needed */

#include <sys/types.h>
#include <kassert.h>

#include "../kio.h"

#include "heap.h"
#include "kmalloc.h"

/* In paging.c */
extern struct page_directory_t* cur_dir;

/* Create a new heap */
void create_heap(struct heap_t* heap, uint32_t start, size_t minsize,   \
                 size_t maxsize, int svisor, int ronly)
{    
    /* Start address is page-aligned */
    if (start & 0x00000FFF != 0)
    {
        start &= 0xFFFFF000;
        start += 0x1000;
    }
    
    /* End address is page-aligned */
    if (maxsize & 0x00000FFF != 0)
    {
        maxsize &= 0xFFFFF000;
        maxsize += 0x1000;
    }
    
    heap->maxsize = maxsize;
    heap->minsize = minsize;
    heap->size = 0;
    
    heap->start = (void*)start;
    
    heap->svisor = svisor;
    heap->ronly = ronly;

    heap->start->size = heap->minsize - sizeof(struct memnode_t);
    heap->start->res = 0;
    heap->start->prev = NULL;
    heap->start->next = heap->end;

    while (heap->size < heap->minsize)
    {
        alloc_frame(get_page(heap->start + heap->size, 1, cur_dir),   \
          (heap->svisor ? 1 : 0), (heap->ronly ? 0 : 1));
        heap->size += 0x1000; /* sizeof(page) */
    }

    heap->size = heap->minsize;
    heap->end->size = 0;
    heap->end->res = 1;
    heap->end->prev = NULL; /* from NULL do_alloc() knows that we have not
                             * allocated anything yet */
    heap->end->next = NULL;
     
    kprintf("Created a heap at 0x%p with max size of %u KiB (%d, %d)\n", start, \
            maxsize/1024, svisor, ronly);
}

/* No need to keep the heap at maxsize, so when we run out of space AND
 * the heap size is not maxsize, we can expand it (by one page)*/
static void expand(struct heap_t* heap)
{
    kassert(heap->size + 0x1000 <= heap->maxsize);
    
    alloc_frame(get_page((void*)(heap->end->prev) + heap->end->prev->size \
                         + 0x1000, 1, cur_dir), (heap->svisor ? 1 : 0), \
                         (heap->ronly ? 0 : 1));
    
    /* Last chunk of the heap is free */
    if (heap->end->prev->res == 0)
    {
        heap->end->prev->size += 0x1000;
    }
    else
    {
        struct memnode_t* unalloc =  heap->end->prev + heap->end->prev->size;
        unalloc->res = 0;
        unalloc->size = 0x1000 - sizeof(struct memnode_t);
        
        unalloc->next = heap->end;
        unalloc->prev = heap->end->prev;
        heap->end->prev = unalloc;
        heap->end->prev->next = unalloc;
    }

    heap->size += 0x1000;
}

/* If the heap is way too large, we should contract it */
static void contract(struct heap_t* heap)
{
    heap->end->prev->size -= 0x1000;
    heap->size -=0x1000;
    free_frame(get_page((void*)(heap->end->prev) + heap->end->prev->size, \
                        0, cur_dir));
}
    
void* do_alloc(size_t size, struct heap_t* heap, int align)
{
    /* Standard malloc() is 16-byte aligned */
    if (size & 0x0F)
    {
        size &= 0xFFFFFFF0;
        size += 0x10;
    }
    
    struct memnode_t* curnode = heap->start;
    while (1)
    {
        /* Heap not large enough */
        if (curnode == heap->end)
        {
            /* Can we expand the heap by one page? */
            if ((heap->size + 0x1000) < heap->maxsize)
            {
                expand(heap);
                curnode = curnode->prev;
            }
            else
                return NULL;
        }
        
        if (curnode->res != 0) /* The node is reserved */
        {
            curnode = curnode->next;
            continue;
        }
                
        if (curnode->size >= size) /* Is free and of adequate size */
        {
            break; /* curnode now points to a usable chunk in the heap */
        }            
        
        curnode = curnode->next;
    }
    
    struct memnode_t* unalloc = (void*)curnode + size  \
                                        + sizeof(struct memnode_t);

    if (heap->end->prev == NULL) /* First allocation */
    {
        unalloc->size = heap->minsize - size - (2 * sizeof(struct memnode_t));
        unalloc->next = heap->end;
    }
    else
    {
        unalloc->size = curnode->size - size - sizeof(struct memnode_t);
        unalloc->next = curnode->next;
    }

    unalloc->res = 0;
    
    unalloc->prev = curnode;
    unalloc->next->prev = unalloc;

    curnode->next = unalloc;
    curnode->size = size;
    curnode->res = 1;
    
    return (void*)((void*)curnode + sizeof(struct memnode_t));    
}

/* Unify continuous free space following the freed memory node */
static void unify_fwd(struct memnode_t* ptr)
{
    ptr->size += ptr->next->size + sizeof(struct memnode_t);

    ptr->next = ptr->next->next;
    ptr->next->prev = ptr;
}

/* Unify continuous free space before the freed memory node */
static void unify_bwd(struct memnode_t* ptr)
{
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;

    ptr->prev->size += ptr->size + sizeof(struct memnode_t);
}

void do_free(void* ptr, struct heap_t* heap)
{
    /* The given ptr points to the beginning of usable space rather than
     * to the beginning of the memnode_t struct */
    ptr -= sizeof(struct memnode_t);

    /* Mark as free */
    ((struct memnode_t*)ptr)->res = 0;    

    /* Unify free memory nodes */
    if (((struct memnode_t*)ptr)->next->res == 0)
        unify_fwd((struct memnode_t*)ptr);

    if (((struct memnode_t*)ptr)->prev->res == 0)
        unify_bwd((struct memnode_t*)ptr);

    /* there is too much empty space in the end of the -> contract */
    while (heap->size > heap->minsize && heap->end->prev->size >= 0x1000)
    {
        contract(heap);
    }
}


static void print_heap_t(struct heap_t* heap)
{
    kprintf("Heap at %p has a max size of %u bytes, perms %d, %d \n", \
            heap, heap->maxsize, heap->svisor, heap->ronly);
    kprintf("Its first memnode_t is at %p and the last one is at %p\n", \
            heap->start, heap->end);
}

static void print_memnode_t(struct memnode_t* memnode)
{
    kprintf("MEMNODE: %p %u %u, %s; %p %p\n", memnode,             \
            memnode->size, memnode->size+sizeof(struct memnode_t), \
            memnode->res == 0 ? "free" : "used", memnode->prev,    \
            memnode->next);
}

void dump_heap(struct heap_t* heap)
{
    kprintf("**HEAP DUMP**\n");
    struct memnode_t* ptr = heap->start;
    while (ptr)
    {
        print_memnode_t(ptr);
        ptr = ptr->next;    
    }
    kprintf("\n");
}
