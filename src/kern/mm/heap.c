/* This file is a part of the RazOS project
 *
 * heap.c -- heap allocation, freeing etc
 *
 * Razbit 2014 */


#include <stdint.h>
#include <stddef.h>
#include <kassert.h>

#include "../kio.h"

#include "heap.h"
#include "kmalloc.h"

/* In paging.c */
extern struct page_directory_t* cur_dir;

/* Allocates pages (frames) taken by the heap */
static void alloc_heap(struct heap_t* heap, size_t oldsize, size_t newsize);

/* No need to keep the heap at maxsize, so when we run out of space AND
 * the heap size is not maxsize, we can expand it */
static void expand(size_t newsize, struct heap_t* heap);

/* Allocates pages (frames) taken by the heap */
static void alloc_heap(struct heap_t* heap, size_t oldsize, size_t newsize);

/* Frees pages relased by contracting the heap */
static void free_heap(struct heap_t* heap, size_t newsize);

/* Create a new heap */
struct heap_t* create_heap(uint32_t start, size_t maxsize, int svisor, int ronly)
{
    struct heap_t* heap = kmalloc(sizeof(struct heap_t));
    
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
    
    heap->maxsize = maxsize; /* Maximum **available** memory, thus
                              * excludes the heap-ending dummy memnode_t */
    heap->size = MIN_HEAP_SIZE;

    heap->start = (void*)start;
    heap->end = kmalloc(sizeof(struct memnode_t));

    heap->svisor = svisor;
    heap->ronly = ronly;

    heap->start->size = MIN_HEAP_SIZE - sizeof(struct memnode_t);
    heap->start->res = 0;
    heap->start->prev = NULL;
    heap->start->next = heap->end;

    alloc_heap(heap, 0, MIN_HEAP_SIZE);
    
    heap->end->size = 0;
    heap->end->res = 1;
    heap->end->prev = NULL; /* from NULL alloc() knows that we have not
                             * allocated anything yet */
    heap->end->next = NULL;
     
    kprintf("Creating a heap at %p with max size of %p (%d, %d)\n", start, \
            maxsize, svisor, ronly);
        
    return heap;
}

/* Allocates pages (frames) taken by the heap */
static void alloc_heap(struct heap_t* heap, size_t oldsize, size_t newsize)
{
    newsize += 0x1000;
    while (oldsize <= newsize)
    {
        alloc_frame(get_page(heap->start + oldsize, 1, cur_dir), \
                    (heap->svisor ? 1 : 0), (heap->ronly ? 0 : 1));
        oldsize += 0x1000; /* sizeof(page) */
    }
}

/* Frees pages relased by contracting the heap */
static void free_heap(struct heap_t* heap, size_t newsize)
{
    size_t oldsize = heap->end - heap->start + sizeof(struct memnode_t);

    size_t i = oldsize - 0x1000;
    while (newsize < i)
    {
        free_frame(get_page(heap->start + i, 0, cur_dir));
        i -= 0x1000; /* sizeof(page) */
    }
}

/* No need to keep the heap at maxsize, so when we run out of space AND
 * the heap size is not maxsize, we can expand it */
static void expand(size_t newsize, struct heap_t* heap)
{
    kassert(newsize > heap->end - heap->start);
    kassert(newsize <= heap->maxsize);

    /* Page-align */
    if ((newsize + sizeof(struct memnode_t)) & 0x00000FFF != 0)
    {
        newsize &= 0xFFFFF000;
        newsize += 0x1000;
        newsize -= sizeof(struct memnode_t); /* 0x10 bytes */
    }

    alloc_heap(heap, (size_t)(heap->end - heap->start), newsize);

    struct memnode_t* new_end_node = heap->start + newsize;

    new_end_node->prev = heap->end->prev;

    heap->end = new_end_node;

    heap->end->size = 0;
    heap->end->res = 1;
    heap->end->next = NULL;

    /* Check if the last chunk of the heap is reserved (malloc'd) */
    if (heap->end->prev->res)
    {
        struct memnode_t* new_emptyness_allocator = heap->end->prev \
            + heap->end->prev->size + sizeof(struct memnode_t);
        new_emptyness_allocator->size = 0;
        new_emptyness_allocator->res = 0;
        new_emptyness_allocator->prev = heap->end->prev;
        heap->end->prev->next = new_emptyness_allocator;
        heap->end->prev = new_emptyness_allocator;
    }
    
    heap->end->prev->next = heap->end;
    heap->end->prev->size = heap->end - heap->end->prev \
        - sizeof(struct memnode_t);
}

/* If the heap is way too large, we should contract it */
static void contract(size_t newsize, struct heap_t* heap)
{
    kassert(newsize < heap->end - heap->start);

    if (newsize < MIN_HEAP_SIZE)
        newsize = MIN_HEAP_SIZE;

    /* Page-align */
    if ((newsize + sizeof(struct memnode_t)) & 0x00000FFF != 0)
    {
        newsize &= 0xFFFFF000;
        newsize += 0x1000;
        newsize -= sizeof(struct memnode_t); /* 0x10 bytes */
    }

    struct memnode_t* new_end_node = heap->start + newsize;
    new_end_node->res = 1;
    new_end_node->size = 0;
    new_end_node->next = NULL;

    new_end_node->prev = heap->end->prev;
    heap->end->prev->next = new_end_node;
    heap->end = new_end_node;
    heap->end->prev->size = heap->end - heap->end->prev \
        - sizeof(struct memnode_t);

    free_heap(heap, newsize);
}
    
void* alloc(size_t size, struct heap_t* heap, int align)
{
    struct memnode_t* curnode = heap->start;
    while (1)
    {
        if (curnode->res != 0) /* The node is reserved */
        {
            curnode = curnode->next;
            continue;
        }
        
        if (curnode->size >= size) /* Is free and of adequate size */
        {
            break; /* curnode now points to a usable chunk in the heap */
        }            

        if (curnode->next == NULL)
        {
            /* Heap not large enough */
            return NULL;
        }
        
        curnode = curnode->next;
    }
    
    struct memnode_t* unalloc = (void*)curnode + size  \
                                        + sizeof(struct memnode_t);

    if (heap->end->prev == NULL) /* First allocation */
    {
        unalloc->size = MIN_HEAP_SIZE - size - (2 * sizeof(struct memnode_t));
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
    ptr->next = ptr->next->next;
    ptr->next->prev = ptr;

    ptr->size = ptr->next - ptr - sizeof(struct memnode_t);
}

/* Unify continuous free space before the freed memory node */
static void unify_bwd(struct memnode_t* ptr)
{
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;

    ptr->prev->size = (void*)(ptr->next) - (void*)(ptr->prev) - sizeof(struct memnode_t);
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

    dump_heap(heap);
}


static void print_heap_t(struct heap_t* heap)
{
    kprintf("Heap at %p has a max size of %p bytes, perms %d, %d \n", \
            heap, heap->maxsize, heap->svisor, heap->ronly);
    kprintf("Its first memnode_t is at %p and the last one is at %p\n", \
            heap->start, heap->end);
}

static void print_memnode_t(struct memnode_t* memnode)
{
    kprintf("MEMNODE: %p %p %p, %s; %p %p\n", memnode,             \
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
