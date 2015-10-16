/* This file is a part of the RazOS project
 *
 * paging.h -- paging related stuff
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#ifndef PAGING_H
#define PAGING_H

#include <sys/types.h>
#include <asm/multiboot.h>

#define PAGE_SIZE 4096

#define PE_FLAG_MASK (PAGE_SIZE - 1)
#define PE_ADDR_MASK (~PE_FLAG_MASK)

/* Memory map (see README.md) */
#define KERNEL_STACK_BEGIN 0x0FC00000
#define KERNEL_STACK_END   0x0FFFFFFC
#define USER_MEMORY_BEGIN  0x10000000
#define USER_STACK_END	   0xFFC00000
#define CUR_PG_DIR		   0xFFFFF000
#define CUR_PG_TABLE_BASE  0xFFC00000

/* Page flags */
#define PE_PRESENT 0x1
#define PE_RW 0x2
#define PE_USER 0x4

/* Bit 31 in cr0 enables paging */
#define FL_PAGING_ENABLED (1 << 31)

/* Set page_dir as current page directory */
void set_page_directory(uint32_t page_dir);

/* Allocate a frame */
uint32_t page_alloc();

/* Free a frame */
void page_free(uint32_t addr);

/* Map a page to a frame */
void page_map(uint32_t virt_page, uint32_t phys_page, uint32_t flag);

/* Unmap a page */
void page_unmap(uint32_t page);

/* Return physical address of memory pointed to by virt */
uint32_t virt_to_phys(uint32_t virt);

/* Return non-zero if page is present AND mapped to USER */
int page_mapped_to_user(uint32_t virt);

/* Figure out the beginning of allocatable memory */
void paging_set_allocatable_start(uint32_t* addr);

/* Initialize paging */
void paging_init(struct multiboot_info* mb);

#endif /* PAGING_H */
