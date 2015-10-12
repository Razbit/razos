/* This file is a part of the RazOS project
 *
 * kernel_paging.h -- kernel-specific paging stuff
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#ifndef KERNEL_PAGE_H
#define KERNEL_PAGE_H

#include <sys/types.h>

/* Initialize kernel's paging */
void kernel_page_init(uint32_t begin, uint32_t _end);

/* Allocate a page for kernel use */
void* kernel_page_alloc();

/* Allocate a page for kernel use. Set contents to 0 */
void* kernel_page_alloc_zeroed();

/* Free a page */
void kernel_page_free(void* addr);

#endif /* KERNEL_PAGE_H */
