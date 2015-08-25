/* This file is a part of the RazOS project
 *
 * paging.h -- paging related stuff
 *
 * Razbit 2014 */

#ifndef PAGING_H
#define PAGING_H

#include <sys/types.h>
#include "../interrupt/isr.h"

/* A page table entry */
struct page_t
{
    uint32_t present   : 1;  /* Page is present in memory */
    uint32_t rw        : 1;  /* R/W if set, ro otherwise */
    uint32_t user      : 1;  /* Supervisor-only if unset */
    uint32_t accessed  : 1;  /* Accessed since last refresh */
    uint32_t dirty     : 1;  /* Written to since last refresh */
    uint32_t reserved  : 7;  /* Unused bits */
    uint32_t frame     : 20; /* Frame (physical addr of the page) */
};

/* A page table is a page directory entry */
struct page_table_t
{
    struct page_t pages[1024];
};

struct page_directory_t
{
    /* Array of pointers to page tables */
    struct page_table_t* tables[1024];

    /* Same as above, but with pysical locations for loading into the
     * CR3 register */
    uint32_t tables_physaddr[1024];

    /* The physical location of the above tables_physaddr. Needed when
     * cloning page directories so we can tell the CPU where to find
     * the new/cloned page directory */
    uint32_t physaddr;
};


/* Set up the page directory, load it to CR3 and enable paging*/
void init_paging();

/* Loads the specified page directory to the CR3 */
void switch_page_directory(struct page_directory_t* new_pdir);

/* Retrieve a pointer to the page. If create == 1, create the associated
 * page table if not already created */
struct page_t* get_page(uint32_t address, int create,   \
                        struct page_directory_t* dir);

/* Allocates a frame -- map a page to a frame (to a physical address) */
void alloc_frame(struct page_t* page, uint8_t is_kern, uint8_t is_rw);

/* Frees a frame */
void free_frame(struct page_t* page);

/* Page fault handler */
void page_fault(struct register_t regs);

#endif /* PAGING_H */
