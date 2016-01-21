/* This file is a part of the RazOS project
 *
 * kernel_paging.c -- kernel-specific paging stuff
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <panic.h>
#include <string.h>
#include <util.h>
#include <errno.h>

#include "paging.h"

#include "kernel_page.h"

typedef union kernel_page
{
	union kernel_page* next;
	uint8_t mem[PAGE_SIZE];
} kernel_page_t;

static kernel_page_t* allocated_to = NULL;
static kernel_page_t* end = NULL;
static kernel_page_t* next_free = NULL;

void kernel_page_init(uint32_t begin, uint32_t _end)
{
	allocated_to = (kernel_page_t*)begin;
	end = (kernel_page_t*)_end;
}

/* Allocate a page for kernel use */
void* kernel_page_alloc()
{
	if (next_free)
	{
		kernel_page_t* page = next_free;
		next_free = next_free->next;
		return page;
	}

	/* Out of memory */
	if (allocated_to >= end)
	{
		errno = ENOMEM;
		return NULL;
	}

	page_map((uint32_t)allocated_to, page_alloc(), PE_PRESENT | PE_RW);

	void* ret = allocated_to++;

	return ret;
}

/* Allocate a page at addr for kernel use */
void* kernel_page_alloc_at(uint32_t addr)
{
	/* Out of memory */
	if ((kernel_page_t*)addr >= end)
	{
		errno = ENOMEM;
		return NULL;
	}

	uint32_t page_start = round_down(addr, PAGE_SIZE);
	page_map(page_start, page_alloc(), PE_PRESENT | PE_RW);

	return (void*)page_start;
}

/* Allocate a page at addr for kernel use, set contents to 0 */
void* kernel_page_alloc_zeroed_at(uint32_t addr)
{
	void* page = kernel_page_alloc_at(addr);
	if (page != NULL)
		memset(page, 0, PAGE_SIZE);
	return page;
}

/* Allocate a page for kernel use. Set contents to 0 */
void* kernel_page_alloc_zeroed()
{
	void* page = kernel_page_alloc();
	if (page != NULL)
		memset(page, 0, PAGE_SIZE);
	return page;
}

/* Free a page */
void kernel_page_free(void* addr)
{
	kernel_page_t* page = addr;
	page->next = next_free;
	next_free = page;
}
