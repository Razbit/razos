/* This file is a part of the RazOS project
 *
 * kmalloc.h -- defines kernel-world malloc() family
 *
 * Razbit 2014 */

#ifndef KMALLOC_H
#define KMALLOC_H

#include <sys/types.h>

/* Page aligned */
void* kmalloc_a(size_t size);

/* Returns physical address */
void* kmalloc_p(size_t size, uint32_t* physaddr);

/* Page aligned and returns physical address */
void* kmalloc_ap(size_t size, uint32_t* physaddr);

/* Normal malloc() */
void* kmalloc(size_t size);

/* Normal free() */
void kfree(void* ptr);

#endif /* KMALLOC_H */
