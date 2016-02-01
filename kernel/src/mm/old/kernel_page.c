/* This file is a part of the RazOS project
 *
 * kernel_paging.c -- kernel-specific paging stuff
 *
 * Razbit 2015, 2016 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <panic.h>
#include <string.h>
#include <util.h>
#include <errno.h>
#include <console.h>
#include "paging.h"

#include "kernel_page.h"

static uint32_t kheap_begin = 0;
static uint32_t kheap_end = 0;

void kernel_page_init(uint32_t begin)
{
	kprintf("Kernel heap starts at 0x%p\n", begin);
	kheap_begin = begin;
	kheap_end = begin;

	/* Mark pages taken by the kernel image as used */
	while(frame_alloc() < kheap_begin);
}

/* Allocate a page for kernel use */
void* kernel_page_alloc()
{
	if (kheap_end < KERNEL_STACK_BEGIN)
	{
		void* ret = page_map(kheap_end, frame_alloc(), \
		                     PE_RW | PE_PRESENT);
		if (ret == NULL)
			return NULL; /* page_map sets errno */

		kheap_end += PAGE_SIZE;
		return ret;
	}
	else
	{
		errno = ENOMEM;
		return NULL;
	}
}

/* Allocate a page for kernel use. Set contents to 0 */
void* kernel_page_alloc_zeroed()
{
	void* page = kernel_page_alloc();
	if (page != NULL)
		memset(page, 0, PAGE_SIZE);
	return page;
}

/* Free the last allocated page */
void kernel_page_free()
{
	uint32_t addr = kheap_end - PAGE_SIZE;
	if (page_flags(addr) & PE_PRESENT)
	{
		page_free(addr);
		kheap_end -= PAGE_SIZE;
	}
}
