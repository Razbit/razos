/* This file is a part of the RazOS project
 *
 * kheap.h -- kernel heap system
 *
 * Razbit 2015 */

#ifndef KHEAP_H
#define KHEAP_H

#include <sys/types.h>
#include <console.h>

#include "paging.h"
#include "kernel_page.h"

#define KHEAP_MAX KERNEL_STACK_BEGIN

/* Internals of the kmalloc() -family
 * We align the start address to [align] bytes (multiple of 16)*/
void* do_kmalloc(size_t size, size_t align);

/* Internals of the kfree() */
void do_kfree(void* ptr);

/* Kernel's brk()-ish functionality */
int kbrk(void* addr);

/* Kernel's sbrk()-ish functionality */
void* ksbrk(size_t increment);

void dump_kheap();

#endif /* KHEAP_H */
