/* This file is a part of the RazOS project
 *
 * paging.c -- paging related stuff
 *
 * Razbit 2014 */

#include "paging.h"
#include "kmalloc.h"
#include "heap.h" /* MIN_HEAP_SIZE */
#include "../interrupt/irq.h"
#include "../kio.h"

#include <stdint.h>
#include <panic.h>
#include <string.h>

/* Kernel's page directory */
struct page_directory_t* kernel_dir = 0;

/* Current page directory */
struct page_directory_t* cur_dir = 0;

/* A bitmap of used and unused frames */
uint32_t *frames;
uint32_t nframes;

/* In kmalloc.c */
extern uint32_t placement_addr;
extern struct heap_t* kheap;

/* Macros for the bitmaps */
#define INDEX_FROM_BIT(a) (a/(32))
#define OFFSET_FROM_BIT(a) (a%(32))

/* Kernel heap props */
#define KHEAP_START 0x200000
#define KHEAP_INIT_SIZE MIN_HEAP_SIZE
#define KHEAP_MAX_SIZE 0x3FFFFF /* 4 MB */

static void set_frame(uint32_t addr)
{
    uint32_t frame = addr/0x1000;
    uint32_t index = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    frames[index] |= (0x1 << offset);
}

static void clear_frame(uint32_t addr)
{
    uint32_t frame = addr/0x1000;
    uint32_t index = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    frames[index] &= ~(0x1 << offset);
}

static uint32_t test_frame(uint32_t addr)
{
    uint32_t frame = addr/0x1000;
    uint32_t index = INDEX_FROM_BIT(frame);
    uint32_t offset = OFFSET_FROM_BIT(frame);
    return (frames[index] & (0x1 << offset));
}

static uint32_t find_free_frame()
{
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF) /* No free frames -> ret */
        {
            /* At least one free page */
            for (j = 0; j < 32; j++)
            {
                if (!(frames[i] & (0x1 << j)))
                    return i*32+j;
            }
        }
    }
}

void alloc_frame(struct page_t* page, uint8_t is_kern, uint8_t is_rw)
{
    if (page->frame != 0)
        return; /* The frame was already allocated */

    uint32_t index = find_free_frame();
    if (index == (uint32_t)-1)
        panic("No free frames");

    set_frame(index*0x1000);
    page->present = 1;
    page->rw = is_rw ? 1 : 0;
    page->user = is_kern ? 1 : 0;
    page->frame = index;
}

void free_frame(struct page_t* page)
{
    uint32_t frame;
    if (!(frame = page->frame))
        return; /* The page didn't have a frame allocated */

    clear_frame(frame);
    page->frame = 0;
}

void init_paging()
{
    /* This is the size of physical memory */
    uint32_t mem_end_page = 0x1000000; /* 16MB */

    kprintf("Initializing paging with 0x%X bytes of physical memory\n", \
            mem_end_page);
    nframes = mem_end_page / 0x1000;
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
    
    /* Create a page directory */
    kernel_dir = (struct page_directory_t*)         \
        kmalloc_a(sizeof(struct page_directory_t));
    memset(kernel_dir, 0, sizeof(struct page_directory_t));
    cur_dir = kernel_dir;

    /* Map virtual addresses to physical ones
     * These are already allocated by the kernel
     * Allocate one extra page for the kernel heap */
    int i = 0;
    while (i < placement_addr+0x1000) 
    {
        /* Kernel code readable but not writeable from user-space */
        alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
        i += 0x1000;
    }

    kprintf("Setting kernel heap at 0x%X \n", KHEAP_START);
    /* Map the initial kernel heap area */
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INIT_SIZE; i += 0x1000)
        alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
      
    /* Page fault handler */
    install_isr_handler(14, &page_fault);

    /* Enable paging */
    switch_page_directory(kernel_dir);

    kheap = create_heap(KHEAP_START, KHEAP_MAX_SIZE, 0, 0);
    
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %0, %%cr0":: "r"(cr0));
}

void switch_page_directory(struct page_directory_t* new_pdir)
{
    cur_dir = new_pdir;
    __asm__ __volatile__("mov %0, %%cr3":: "r"(&new_pdir->tables_physaddr));
}

struct page_t* get_page(uint32_t address, int create,   \
                        struct page_directory_t* dir)
{
    address /= 0x1000; /* Turn to an index */
    uint32_t table_index = address / 1024;
    if (dir->tables[table_index]) /* Table is already assigned */
    {
        return &dir->tables[table_index]->pages[address%1024];
    }
    else if (create)
    {
        uint32_t tmp;
        dir->tables[table_index] = (struct page_table_t*)   \
            kmalloc_ap(sizeof(struct page_table_t), &tmp);
        memset(dir->tables[table_index], 0, 0x1000);
        dir->tables_physaddr[table_index] = tmp | 0x7; /* P, RW, USER */
        return &dir->tables[table_index]->pages[address%1024];
    }
    else
        return 0;
}

void page_fault(struct register_t regs)
{
    /* Faulting address is in CR2 */
    uint32_t faulty_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r"(faulty_address));

    /* Parse error code */
    int pres = !(regs.err_code & 0x1); /* Page not present */
    int rw = regs.err_code & 0x2;      /* Write */
    int user = regs.err_code & 0x4;    /* user-mode */
    int res = regs.err_code & 0x08;    /* Overwrite CPU-reserved bits */
    int id = regs.err_code & 0x10;     /* Instruction fetch */

    kprintf("PAGE FAULT (0x%X at %p\n", regs.err_code, faulty_address);
    panic("PAGE FAULT");
}
