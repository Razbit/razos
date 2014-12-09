/* This file is a part of the RazOS project
 *
 * heap.h -- heap allocation, freeing etc
 *
 * Razbit 2014 */

#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

/* Minimum heap size is 512 KB */
#define MIN_HEAP_SIZE (0x80000 - sizeof(struct memnode_t))

struct memnode_t
{
    size_t size;            /* Size of the allocated chunk */
    int res;                /* Used or free? */
    struct memnode_t* prev; /* Previous chunk */
    struct memnode_t* next; /* Next chunk */
};

struct heap_t
{
    size_t maxsize;          /* maximum size of the heap */
    uint8_t svisor;              /* Supervisor-only? */
    uint8_t ronly;               /* Read-only? */
    struct memnode_t* start; /* First chunk of memory */
    struct memnode_t* end;   /* The dummy end node */
};

struct heap_t* create_heap(uint32_t start, size_t maxsize, int svisor, int ronly);
void* alloc(size_t size, struct heap_t* heap, int align);

#endif /* HEAP_H */
