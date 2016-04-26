/* paging.h -- internals of the paging system */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#ifndef PAGING_H
#define PAGING_H

#include <sys/types.h>
#include <asm/multiboot.h>

/* Memory map (see memory_map.md) */
/* Kheap starts at the first page table after the kernel image,
 * giving a maximum size of 244 MiB (ends at 0x0fbfffff) */
#define KHEAP_END      (KSTACK_BEGIN - 1)
#define KSTACK_BEGIN   0x0FC00000 /* Kernel stack */
#define KSTACK_END     0x0FFFFFF0 /* 4 MiB, 16 byte align */
#define UMEM_BEGIN     0x10000000 /* Starts at 256 MiB */
#define UCODE_BEGIN    UMEM_BEGIN
/* Uheap starts at the first page table after the kernel image */
#define UHEAP_END      (USTACK_BEGIN - 1)
#define USTACK_BEGIN   0xFFC00000 /* User stack */
#define USTACK_END     0xFFFFFFF0 /* 4 Mib, 16 byte align */

/* x86-specific 32-bit paging structures and definitions */
/* Page table entry, page dir entry flags */
#define PF_PRES  (1 << 0)
#define PF_RW    (1 << 1)
#define PF_USER  (1 << 2)
#define PF_WTC   (1 << 3)
#define PF_NOC   (1 << 4)
#define PF_ACC   (1 << 5)
#define PF_DIRTY (1 << 6)
#define PF_SIZE  (1 << 7)
#define PF_GLO   (1 << 8)

#define PAGE_SIZE 4096
#define PAGE_FL_MASK (PAGE_SIZE - 1)
#define PAGE_ADDR_MASK (~PAGE_FL_MASK)

/* Page table entry -- describes a single page */
struct page_t
{
	uint32_t present : 1;    /* Page present in memory */
	uint32_t rw : 1;         /* Page writable */
	uint32_t user : 1;       /* User-accessible */
	uint32_t wt_caching : 1; /* Write-through caching */
	uint32_t nocache : 1;    /* Caching disabled */
	uint32_t accessed : 1;   /* Has been accessed */
	uint32_t dirty : 1;      /* Has been written to */
	uint32_t zero : 1;       /* Always zero */
	uint32_t global : 1;     /* Global: do not flush in TLB flush */
	uint32_t avail : 3;      /* Bits not used by CPU */
	uint32_t frame : 20;     /* Physical address of frame */
};

/* Page table -- an array of 1024 pages. Align to 4K! */
struct page_tab_t
{
	struct page_t entry[1024];
};

struct pg_dir_entry_t
{
	uint32_t present : 1;    /* Page present in memory */
	uint32_t rw : 1;         /* Page writable */
	uint32_t user : 1;       /* User-accessible */
	uint32_t wt_caching : 1; /* Write-through caching */
	uint32_t nocache : 1;    /* Caching disabled */
	uint32_t accessed : 1;   /* Has been accessed */
	uint32_t zero : 1;       /* Always zero */
	uint32_t size : 1;       /* 0 = 4K, 1 = 4M pages */
	uint32_t global : 1;     /* Global: do not flush in TLB flush */
	uint32_t avail : 3;      /* Bits not used by CPU */
	uint32_t table : 20;     /* Physical address of the page_tab_t */
};

/* A page directory -- an array of 1024 page tables. Align to 4K! */
struct page_dir_t
{
	/* Used when modifying a page table entry */
	struct page_tab_t* tables[1024];
	/* Used when modifying a page dir entry.
	 * The *physical* address of the first pg_dir_entry is put in cr3 */
	struct pg_dir_entry_t tables_phys[1024];
};

extern struct page_dir_t* cur_page_dir;

/* Create a page directory */
struct page_dir_t* create_page_dir();

/* Clone a page directory */
void clone_page_dir(struct page_dir_t* new, struct page_dir_t* old);

/* Clear a page directory */
void clear_page_dir(struct page_dir_t* page_dir);

/* Load the specified page directory to CR3 */
void set_page_dir(struct page_dir_t* page_dir);

/* Get physical address of page dir for loading to cr3 */
uint32_t get_page_dir_phys(struct page_dir_t* page_dir);

/* Allocate a frame, return its physical address */
uint32_t frame_alloc();

/* Free a frame. Takes physical frame address */
void frame_free(uint32_t addr);

/* Map frame to address, set flags */
void* page_map(uint32_t addr, uint32_t frame, uint32_t flags, \
               struct page_dir_t* page_dir);

/* Convert virtual address to physical address */
uint32_t get_phys(uint32_t addr, struct page_dir_t* page_dir);

/* Get page and page table flags */
uint32_t page_flags(uint32_t addr, struct page_dir_t* page_dir);

/* Initialize paging */
void paging_init(struct multiboot_info* mb);

/* Grow identity-mapped region. If addr == 0, return current end */
void* set_alloc_start(void* addr);

/* Get the number of used frames */
uint32_t get_used_frames();

#endif /* PAGING_H */
