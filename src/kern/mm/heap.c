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
static void alloc_heap(struct heap_t* heap, size_t newsize);

/* No need to keep the heap at maxsize, so when we run out of space AND
 * the heap size is not maxsize, we can expand it */
static void expand(size_t newsize, struct heap_t* heap);

static void print_heap_t(struct heap_t* heap)
{
    kprintf("Heap at %p has a max size of %p bytes, perms %d, %d \n", \
            heap, heap->maxsize, heap->svisor, heap->ronly);
    kprintf("Its first memnode_t is at %p and the last one is at %p\n", \
            heap->start, heap->end);
}

static void print_memnode_t(struct memnode_t* memnode)
{
    kprintf("MEMNODE: %p %p, %s; %p %p\n", memnode,             \
            memnode->size, memnode->res == 0 ? "free" : "used", \
            memnode->next, memnode->prev);
}

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
    if ((maxsize + sizeof(struct memnode_t)) & 0x00000FFF != 0)
    {
        maxsize &= 0xFFFFF000;
        maxsize += 0x1000;
        maxsize -= sizeof(struct memnode_t); /* 0x10 bytes */
    }
    
    heap->maxsize = maxsize; /* Maximum **available** memory, thus
                              * excludes the heap-ending dummy memnode_t */

    /* Initial memory node */
    struct memnode_t* start_node = (void*)start;
    /* The terminating dummy memory node */
    struct memnode_t* end_node = (void*)(start + MIN_HEAP_SIZE);

    heap->start = start_node;
    heap->end = end_node;

    heap->svisor = svisor;
    heap->ronly = ronly;

    heap->start->size = MIN_HEAP_SIZE;
    heap->start->res = 0;
    heap->start->prev = NULL;
    heap->start->next = heap->end;

    heap->end->size = 0;
    heap->end->res = 1;
    heap->end->prev = heap->start;
    heap->end->next = NULL;

    alloc_heap(heap, MIN_HEAP_SIZE + sizeof(struct memnode_t));

    #ifdef _DEBUG
    print_heap_t(heap);
    print_memnode_t(heap->start);
    print_memnode_t(heap->end);
    #endif
    
    kprintf("Creating a heap at %p with max size of %p (%d, %d)\n", start, \
            maxsize, svisor, ronly);
    
    return heap;
}

/* Allocates pages (frames) taken by the heap */
static void alloc_heap(struct heap_t* heap, size_t newsize)
{
    size_t oldsize = heap->end - heap->start + sizeof(struct memnode_t);
    
    while (oldsize < newsize)
    {
        alloc_frame(get_page(heap->start + oldsize, 1, cur_dir), \
                    (heap->svisor ? 1 : 0), (heap->ronly ? 0 : 1));
        oldsize += 0x1000; /* sizeof(page) */
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

    alloc_heap(heap, newsize);

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

    curnode->size = size;
    curnode->res = 1;
    
    struct memnode_t* unalloc = curnode + size + sizeof(struct memnode_t);
    
    unalloc->size = curnode->next - unalloc - sizeof(struct memnode_t);
    unalloc->res = 0;
    unalloc->next = curnode->next;
    unalloc->prev = curnode;
    unalloc->next->prev = unalloc;
    curnode->next = unalloc;

    print_memnode_t(curnode);
    print_memnode_t(unalloc);
    return (void*)(curnode + sizeof(struct memnode_t));    
}
