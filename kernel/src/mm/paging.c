/* This file is a part of the RazOS project
 *
 * paging.c -- paging related stuff
 *
 * Razbit 2015, 2016 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include "paging.h"


/* Use a bitmap to keep track of used/free frames */
static uint32_t frames[0x8000]; /* 0x8000 * 32 -> pages for 4 GiB */

#define frame_index(a) ((a)/32)  /* Index in frames[] */
#define frame_offset(a) ((a)%32) /* Bit number in the uint32_t */

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
	/* This could be done just once, but I'll let this be here.. */
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0) :: "memory");
	cr0 |= FL_PAGING_ENABLED;
	__asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0) : "memory");
}

/* Mark frame as used */
static void set_frame(uint32_t frame)
{
	frames[frame_index(frame)] |= (1 << frame_offset(frame));
}

/* Mark frame as free */
static void clear_frame(uint32_t frame)
{
	frames[frame_index(frame)] &= ~(1 << frame_offset(frame));
}

/* Find first free frame */
static uint32_t find_free_frame()
{
	uint32_t i, j;
	for (i = 0; i < frame_index(nframes); i++)
		if (frames[i] != 0xFFFFFFFF)
			for (j = 0; j < 32; j++)
				if ((frames[i] & ((uint32_t)1 << j)) == 0)
					return i * 32 + j;

	/* If no free frame was found, return -1 */
	return (uint32_t)-1;
}

uint32_t allocated_frames()
{
	uint32_t ret = 0;
	for (uint32_t i = 0; i < frame_index(nframes); i++)
		for(uint32_t j = 0; j < 32; j++)
			if ((frames[i] & ((uint32_t)1 << j)) == 1)
				ret++;
	return ret;
}

/* Allocate a frame */
uint32_t frame_alloc()
{
	uint32_t index = find_free_frame();
	if (index == (uint32_t)-1)
	{
		errno = ENOMEM;
		return 0;
	}

	/* Mark frame as used */
	set_frame(index);

	return index*PAGE_SIZE;
}

/* Free a frame */
void frame_free(uint32_t addr)
{
	clear_frame(addr/PAGE_SIZE);
}

void* page_map(uint32_t virt_page, uint32_t phys_page, uint32_t flags)
{
	/* If we really try to map a page, but frame_alloc() gave us 0 */
	if (phys_page == 0 && flags & PE_PRESENT)
	{
		errno = ENOMEM;
		return NULL;
	}

	/* Page directories mapped recursively to themselves */
	uint32_t* cur_page_dir = (uint32_t*)CUR_PG_DIR;

	size_t page_dir_index = (virt_page / PAGE_SIZE) / 1024;
	size_t page_table_index = (virt_page / PAGE_SIZE) % 1024;

	uint32_t* page_table = \
		(uint32_t*)(CUR_PG_TABLE_BASE + page_dir_index * PAGE_SIZE);

	uint32_t page_dir_entry = cur_page_dir[page_dir_index];
	if (!(page_dir_entry & PE_PRESENT))
	{
		/* Create the page table if not present */
		uint32_t page = frame_alloc();
		if (page == 0)
		{
			errno = ENOMEM;
			return NULL;
		}

		/* Make page table writable, so we can write to pages
		 * that are specified PE_RW */
		cur_page_dir[page_dir_index] = page|(flags&PE_FLAG_MASK)|PE_RW;
		tlb_flush((uint32_t)page_table);
		memset(page_table, 0, PAGE_SIZE);
	}

	page_table[page_table_index] = \
		(phys_page & PE_ADDR_MASK) | (flags & PE_FLAG_MASK);
	tlb_flush((uint32_t)virt_page);

	return (void*)virt_page;
}

/* Unmap a page */
void page_unmap(uint32_t page)
{
	page_map(page, 0, 0);
}

void page_free(uint32_t page)
{
	frame_free(page);//virt_to_phys(page));
	page_map(page, 0, 0);
}

/* Return physical address of memory pointed to by virt */
uint32_t virt_to_phys(uint32_t virt)
{
	uint32_t* cur_page_dir = (uint32_t*)CUR_PG_DIR;

	size_t dir_i = virt / (PAGE_SIZE * 1024);
	size_t tab_i = (virt / PAGE_SIZE) % 1024;
	size_t offset = virt % PAGE_SIZE;

	/* No such page table allocated */
	if (!(cur_page_dir[dir_i] & PE_PRESENT))
		return 0;

	uint32_t* page_table = \
		(uint32_t*)(CUR_PG_TABLE_BASE + dir_i * PAGE_SIZE);

	/* No such page allocated */
	if (!(page_table[tab_i] & PE_PRESENT))
		return 0;

	return (page_table[tab_i] & PE_ADDR_MASK) + offset;
}

/* Return non-zero if page is present AND mapped to USER */
int page_mapped_to_user(uint32_t virt)
{
	uint32_t dir_index = (virt / PAGE_SIZE) / 1024;
	uint32_t base_index = virt / PAGE_SIZE;

	uint32_t* page_dir = (uint32_t*)CUR_PG_DIR;

	if (!(page_dir[dir_index] & (PE_PRESENT | PE_USER)))
		return 0;

	uint32_t* page_table_base = (uint32_t*)CUR_PG_TABLE_BASE;

	if (!(page_table_base[base_index] & (PE_PRESENT | PE_USER)))
		return 0;

	return 1;
}

/* Return page table and page flags */
uint32_t page_flags(uint32_t virt)
{
	uint32_t dir_index = (virt / PAGE_SIZE) / 1024;
	uint32_t base_index = virt / PAGE_SIZE;

	uint32_t* page_dir = (uint32_t*)CUR_PG_DIR;
	uint32_t* page_table_base = (uint32_t*)CUR_PG_TABLE_BASE;
	
	uint32_t flags = (page_dir[dir_index] & PE_FLAG_MASK) << 16;
	flags |= (page_table_base[base_index] & PE_FLAG_MASK);

	return flags;
}
