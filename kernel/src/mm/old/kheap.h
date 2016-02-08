/* This file is a part of the RazOS project
 *
 * kheap.h -- kernel heap system
 *
 * Razbit 2015, 2016 */

#ifndef KHEAP_H
#define KHEAP_H

#include <sys/types.h>

#include "paging.h"

/* Internals of the kmalloc() -family
 * We align the start address to [align] bytes (multiple of 16) */
void* do_kmalloc(size_t size, size_t align);

/* Internals of the kfree() */
void do_kfree(void* ptr);

void dump_kheap();

#endif /* KHEAP_H */
