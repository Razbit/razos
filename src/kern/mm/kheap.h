/* This file is a part of the RazOS project
 *
 * kheap.c -- kernel heap allocation, freeing etc
 *
 * Razbit 2015 */

#ifndef KHEAP_H
#define KHEAP_H

/* Create a new heap */
void create_kheap(struct heap_t* heap, uint32_t start, size_t size);

/* Heap-mode internals of malloc() and free() */
void* do_kmalloc(size_t size, int align);
void do_kfree(void* ptr);

/* Dumps heap structures */
void dump_kheap();

#endif /* KHEAP_H */
