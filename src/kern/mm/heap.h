/* This file is a part of the RazOS project
 *
 * heap.h -- heap allocation, freeing etc
 *
 * Razbit 2014 */

#ifndef HEAP_H
#define HEAP_H

#include <sys/types.h>

/* Minimum heap size is 512 KB */
#define MIN_HEAP_SIZE 0x80000

struct memnode_t
{
    size_t size;            /* Size of the allocated chunk */
    uint32_t res;            /* Used or free? */
    struct memnode_t* prev; /* Previous chunk */
    struct memnode_t* next; /* Next chunk */
}__attribute__((__packed__));

struct heap_t
{
    size_t maxsize;          /* Maximum size of the heap */
    size_t minsize;          /* Minimum size of the heap */
    size_t size;             /* Current size of the heap */
    uint8_t svisor;          /* Supervisor-only? */
    uint8_t ronly;           /* Read-only? */
    struct memnode_t* start; /* First chunk of memory */
    struct memnode_t* end;   /* The dummy end node */
}__attribute__((__packed__));

/* Create a new heap */
void create_heap(struct heap_t* heap, uint32_t start, size_t minsize,   \
                 size_t maxsize, int svisor, int ronly);

/* Heap-mode internals of malloc() and free() */
void* do_alloc(size_t size, struct heap_t* heap, int align);
void do_free(void* ptr, struct heap_t* heap);

/* Dumps heap structures */
void dump_heap(struct heap_t* heap);

#endif /* HEAP_H */
