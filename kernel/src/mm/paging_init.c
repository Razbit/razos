/* This file is a part of the RazOS project
 *
 * paging_init.c -- paging initialization
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <string.h>
#include <console.h>
#include <asm/multiboot.h>
#include <util.h>

#include "kernel_page.h"
#include "paging.h"

static uint32_t kernel_end;

static uint32_t alloc_zeroed_page()
{
	uint32_t page = page_alloc();
	memset((void*)page, 0, PAGE_SIZE);
	return page;
}

static size_t register_avail_mem_region(struct memory_map* region)
{
	size_t pages_registered = 0;

	for (size_t offset = 0; offset + PAGE_SIZE <= region->length_low; \
	     offset += PAGE_SIZE)
	{
		uint32_t addr = region->base_addr_low + offset;

		/* Can't allocate before the end of the kernel.......... */
		if (addr < kernel_end)
			continue;

		page_free(addr);
		pages_registered++;
	}

	return pages_registered;
}

static size_t register_avail_mem(struct multiboot_info* mb)
{
	struct memory_map* mmap = (void*)mb->mmap_addr;
	size_t pages_registered = 0;
	for (size_t i = 0; i < mb->mmap_length/sizeof(struct memory_map); i++)
	{
		if (mmap[i].type == 1) /* Available? */
			pages_registered += register_avail_mem_region(mmap + i);
	}

	return pages_registered;
}

static void create_page_tables_for_kernel_space(uint32_t* pg_dir)
{
	for (size_t i = 0; i < KERNEL_STACK_END / (PAGE_SIZE * 1024); i++)
	{
		pg_dir[i] = alloc_zeroed_page() | PE_PRESENT | PE_RW;
	}
}

static void identity_map_kernel(uint32_t* page_dir)
{
	/* Leave null-page unmapped, that's why we are starting from
	 * PAGE_SIZE instead of 0 */
	for (uint32_t addr = PAGE_SIZE; addr <= kernel_end; addr += PAGE_SIZE)
	{
		size_t page_dir_index = (addr / 4096) / 1024;
		size_t page_table_index = (addr / 4096) % 1024;
		uint32_t page_dir_entry = page_dir[page_dir_index];
		if (!page_dir_entry)
		{
			page_dir_entry = page_dir[page_dir_index] = \
				alloc_zeroed_page() | PE_PRESENT | PE_RW;
		}

		uint32_t* page_table = (uint32_t*)(page_dir_entry & PE_ADDR_MASK);
		page_table[page_table_index] = addr | PE_PRESENT | PE_RW;
	}
}

static void recursively_map_page_directory(uint32_t* page_dir)
{
	page_dir[1023] = (uint32_t)page_dir | PE_PRESENT | PE_RW;
}

/* Figure out the beginning of allocatable memory */
void paging_set_allocatable_start(uint32_t* addr)
{
	if (addr > kernel_end)
		kernel_end = round_up(addr, PAGE_SIZE);
}

/* Initialize paging */
void paging_init(struct multiboot_info* mb)
{
	extern uint32_t end_of_image; /* In linker.ld, boot.s */
	uint32_t initrd_end = *(uint32_t*)(mb->mods_addr+4);

	paging_set_allocatable_start(&end_of_image);
	paging_set_allocatable_start((uint32_t*)initrd_end);

	size_t pages_registered = register_avail_mem(mb);
	kprintf("%u MiB available useful memory\n", \
	        pages_registered * PAGE_SIZE / 0x100000);

	uint32_t* page_dir = alloc_zeroed_page();

	create_page_tables_for_kernel_space(page_dir);
	identity_map_kernel(page_dir);
	recursively_map_page_directory(page_dir);

	set_page_directory((uint32_t)page_dir);

	kernel_page_init(kernel_end, KERNEL_STACK_BEGIN);
}
