/* This file is a part of the RazOS project
 *
 * kheap.c -- kernel heap allocation, freeing etc
 *
 * Razbit 2015 */

#include <sys/types.h>

#include "kmalloc.h"
#include "heap.h"
#include "kheap.h"

/* The kernel heap resides here */
extern struct heap_t* kheap;

/* Create a new heap */
void create_kheap(struct heap_t* heap, uint32_t start, size_t size)
{    
    /* Start address is page-aligned */
    if (start & 0x00000FFF != 0)
    {
        start &= 0xFFFFF000;
        start += 0x1000;
    }
    
    /* End address is page-aligned */
    if (size & 0x00000FFF != 0)
    {
        size &= 0xFFFFF000;
    }
    
    heap->maxsize = 0;
    heap->minsize = 0;
    heap->size = size;
    
    heap->start = (void*)start;
    
    heap->svisor = 0;
    heap->ronly = 0;

    heap->start->size = heap->size - sizeof(struct memnode_t);
    heap->start->res = 0;
    heap->start->prev = NULL;
    heap->start->next = heap->end;

    heap->end->size = 0;
    heap->end->res = 1;
    heap->end->prev = NULL; /* from NULL kmalloc() knows that we have not
                             * allocated anything yet */
    heap->end->next = NULL;
     
    kprintf("Created kernel heap at 0x%p with size of %u KiB\n", \
            start, size/1024);
}

void* do_kmalloc(size_t size, int align)
{
    /* Standard malloc() is 16-byte aligned */
    if (size & 0x0F)
    {
        size &= 0xFFFFFFF0;
        size += 0x10;
    }
    
    struct memnode_t* curnode = kheap->start;
    while (1)
    {
        /* Heap not large enough */
        if (curnode == kheap->end)
        {
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

    if (kheap->end->prev == NULL) /* First allocation */
    {
        unalloc->size = kheap->size - size - (2 * sizeof(struct memnode_t));
        unalloc->next = kheap->end;
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

void do_kfree(void* ptr)
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
}


static void print_memnode_t(struct memnode_t* memnode)
{
    kprintf("MEMNODE: %p %u %u, %s; %p %p\n", memnode,             \
            memnode->size, memnode->size+sizeof(struct memnode_t), \
            memnode->res == 0 ? "free" : "used", memnode->prev,    \
            memnode->next);
}

void dump_kheap()
{
    kprintf("**KERNEL HEAP DUMP**\n");
    struct memnode_t* ptr = kheap->start;
    while (ptr)
    {
        print_memnode_t(ptr);
        ptr = ptr->next;    
    }
    kprintf("\n");
}
