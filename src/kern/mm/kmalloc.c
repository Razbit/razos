/* This file is a part of the RazOS project
 *
 * kmalloc.c -- kernel-world malloc() family
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <kmalloc.h>

#include "paging.h"
#include "kheap.h"

/* Aligned */
void* kmalloc_a(size_t size, size_t align)
{
	return do_kmalloc(size, align);
}

/* Page aligned */
void* kmalloc_pa(size_t size)
{
	return do_kmalloc(size, PAGE_SIZE);
}

/* Normal malloc() */
void* kmalloc(size_t size)
{
	return do_kmalloc(size, 16);
}

/* Normal free() */
void kfree(void* ptr)
{
	do_kfree(ptr);
}
