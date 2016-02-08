/* This file is a part of the RazOS project
 *
 * kmalloc.h -- defines kernel-world malloc() family
 *
 * Razbit 2014, 2016 */

#ifndef KMALLOC_H
#define KMALLOC_H

#include <sys/types.h>

/* Page aligned */
void* kmalloc_pa(size_t size);

/* Aligned to <align> bytes (multiple of 16)*/
void* kmalloc_a(size_t size, size_t align);

/* Normal malloc(). Aligned to 16 bytes */
void* kmalloc(size_t size);

/* Normal free() */
void kfree(void* ptr);

/* Dump kernel heap structures for debugging */
void dump_kheap();

#endif /* KMALLOC_H */
