/* This file is a part of the RazOS project
 *
 * paging.c -- paging related stuff
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <string.h>

#include "paging.h"

static uint32_t next_free_page = 0;
static uint32_t old_null_page = 0;
static uint32_t* const temp_page_entry = (void*)CUR_PG_TABLE_BASE;


/* Return non-zero if paging is enabled */
static int paging_enabled()
{
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0) :: "memory");
	return (cr0 & FL_PAGING_ENABLED);
}

/* Flush the Translation Lookaside Buffer */
static inline void tlb_flush(uint32_t addr)
{
	__asm__ __volatile__("invlpg (%0)" :: "r"(addr) : "memory");
}

/* Set page_dir as current page directory */
void set_page_directory(uint32_t page_dir)
{
	__asm__ __volatile__("mov %0, %%cr3" :: "r"(page_dir) : "memory");

	/* Make sure paging is enabled */
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0) :: "memory");
	cr0 |= FL_PAGING_ENABLED;
	__asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0) : "memory");
}


/* Used in page_alloc() and page_free() */
static void* page_temp_map(uint32_t phys_page)
{
	/* The temporary page is mapped to the first page in the first
	 * page table, i.e. at virtual address 0 i.e. physical 0xFFC00000 */
	old_null_page = *temp_page_entry;
	*temp_page_entry = phys_page | PE_PRESENT | PE_RW;
	tlb_flush(0);

	return NULL;
}

/* Used in page_alloc() and page_free() */
static void page_temp_unmap()
{
	*temp_page_entry = old_null_page;
	tlb_flush(0);
}

/* Allocate a frame */
uint32_t page_alloc()
{
	uint32_t page = next_free_page;

	if (paging_enabled())
	{
		uint32_t* temp_mapping = page_temp_map(page);
		next_free_page = *temp_mapping;
		page_temp_unmap();
	}
	else
	{
		next_free_page = *(uint32_t*) page;
	}

	return page;
}

/* Free a frame */
void page_free(uint32_t addr)
{
	if (paging_enabled())
	{
		uint32_t* temp_mapping = page_temp_map(addr);
		*temp_mapping = next_free_page;
		page_temp_unmap();
	}
	else
	{
		*(uint32_t*) addr = next_free_page;
	}

	next_free_page = addr;
}

void page_map(uint32_t virt_page, uint32_t phys_page, uint32_t flags)
{
	/* Page directories mapped recursively to themselves */
	uint32_t* cur_page_dir = CUR_PG_DIR;

	size_t page_dir_index = (virt_page / 4096) / 1024;
	size_t page_table_index = (virt_page / 4096) % 1024;

	uint32_t* page_table =								\
		CUR_PG_TABLE_BASE + page_dir_index * PAGE_SIZE;

	uint32_t page_dir_entry = cur_page_dir[page_dir_index];
	if (!(page_dir_entry & PE_PRESENT))
	{
		/* Create the page table if not present */
		cur_page_dir[page_dir_index] =				\
			page_alloc() | (flags & PE_FLAG_MASK);
		tlb_flush((uint32_t)page_table);
		memset(page_table, 0, PAGE_SIZE);
	}

	page_table[page_table_index] =								\
		(phys_page & PE_ADDR_MASK) | (flags & PE_FLAG_MASK);
	tlb_flush((uint32_t)virt_page);
}

/* Unmap a page */
void page_unmap(uint32_t page)
{
	page_map(page, 0, 0);
}

/* Return physical address of memory pointed to by virt */
uint32_t virt_to_phys(uint32_t virt)
{
	uint32_t* cur_page_dir = CUR_PG_DIR;

	size_t page_dir_index = (virt / 4096) / 1024;
	size_t page_table_index = (virt / 4096) % 1024;
	size_t page_offset = virt % 4096;

	/* No such page directory allocated */
	if (!(cur_page_dir[page_dir_index] & PE_PRESENT))
		return 0;

	uint32_t* page_table =								\
		CUR_PG_TABLE_BASE + page_dir_index * PAGE_SIZE;

	uint32_t page_table_entry = page_table[page_table_index];

	/* No such page table allocated */
	if (!(page_table_entry & PE_PRESENT))
		return 0;

	return (page_table_entry & PE_ADDR_MASK) + page_offset;
}

/* Return non-zero if page is present AND mapped to USER */
int page_mapped_to_user(uint32_t virt)
{
	uint32_t dir_index = (virt / PAGE_SIZE) / 1024;
	uint32_t base_index = virt / PAGE_SIZE;

	uint32_t* page_dir = CUR_PG_DIR;

	if (!(page_dir[dir_index] & (PE_PRESENT | PE_USER)))
		return 0;

	uint32_t* page_table_base = CUR_PG_TABLE_BASE;

	if (!(page_table_base[base_index] & (PE_PRESENT | PE_USER)))
		return 0;

	return 1;
}
