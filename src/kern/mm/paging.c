/* This file is a part of the RazOS project
 *
 * paging.c -- paging related stuff
 *
 * Razbit 2014 */

/* Dec. 26, 2014: Heap allocation & freeing work */

#include "paging.h"
#include "kmalloc.h"
#include "kheap.h"
#include "heap.h"
#include "detect.h"
#include "../interrupt/irq.h"
#include "../kio.h"

#include <sys/types.h>
#include <panic.h>
#include <string.h>
#include <asm/multiboot.h>

/* Kernel's page directory */
struct page_directory_t* kernel_dir = 0;

/* Current page directory */
struct page_directory_t* cur_dir = 0;

/* In kmalloc.c */
/* The first free address */
extern uint32_t placement_addr;

/* In kheap.c */
extern struct heap_t* kheap;

/* A bitmap of used and unused frames */
uint32_t *frames;
uint32_t nframes;

/* Macros for the bitmaps */
#define INDEX_FROM_BIT(a) (a/(32))
#define OFFSET_FROM_BIT(a) (a%(32))

/* Total memory available for kernel use */
#define KERNEL_MEMORY 0x2000000 /* 32 MB */

static void set_frame(uint32_t addr)
{
    uint32_t frame = addr/0x1000; /* In which frame the phys addr is */
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
        if (frames[i] != 0xFFFFFFFF)
        {
            /* At least one free page */
            for (j = 0; j < 32; j++)
            {
                if (!(frames[i] & (0x1 << j)))
                    return i*32+j;
            }
        }
    }
    return (uint32_t)-1;
}

void alloc_frame(struct page_t* page, uint8_t is_kern, uint8_t is_rw)
{    
    if (page->frame)
        return; /* The frame was already allocated */
    
    uint32_t index = find_free_frame();
    
    if (index == (uint32_t)-1)
        panic("No free frames");

    /* setup the frame/page */
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

void init_paging(struct multiboot_info* mb)
{
    /* This is the size of physical memory. From multiboot */
    uint32_t mem_end_page = get_avail_mem(mb);

    kprintf("Initializing paging with %u KiB of physical memory\n", \
            mem_end_page/1024);
    
    nframes = mem_end_page / 0x1000;
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
    
    /* Create the kernel page directory */
    uint32_t phys;
    kernel_dir = kmalloc_a(sizeof(struct page_directory_t));
    memset(kernel_dir, 0, sizeof(struct page_directory_t));
    kernel_dir->physaddr = (uint32_t)kernel_dir->tables_physaddr;

    
    /* Allocate space for the kernel heap */
    struct heap_t* heap = kmalloc(sizeof(struct heap_t));
    void* kheap_start = kmalloc_a(KERNEL_MEMORY-placement_addr-sizeof(struct memnode_t)-0x1000-9*0x1000);
    heap->end = kmalloc(sizeof(struct memnode_t));
        
    /* Identity-map virtual addresses to physical ones
     * We map the first KERNEL_MEMORY bytes of memory in the
     * kernel directory -> kheap is of static size */
    int i = 0;
    while (i < placement_addr) 
    {
        /* Kernel code readable but not writeable from user-space */
        alloc_frame(get_page(i, 1, kernel_dir), 0, 0);
        i += 0x1000;
    }
        
    /* Page fault handler */
    install_isr_handler(14, &page_fault);

    /* Set up the kernel heap */   
    create_kheap(heap, kheap_start, (size_t)((uint32_t)heap->end         \
                                       - (uint32_t)kheap_start-0x1000));
    kheap = heap;

    
    /* Enable paging */
    switch_page_dir(kernel_dir);
}

/* Load address of the page directory to CR3 */
void switch_page_dir(struct page_directory_t* new_pdir)
{
    cur_dir = new_pdir;
    __asm__ __volatile__("mov %0, %%cr3":: "r"(&new_pdir->tables_physaddr));
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %0, %%cr0":: "r"(cr0));
}

/* Retrieve a pointer to the page. If create == 1, create the associated
 * page table if not already created */
struct page_t* get_page(uint32_t address, int create,   \
                        struct page_directory_t* dir)
{
    address /= 0x1000; /* Turn to an index in the bitset */
    uint32_t table_index = address / 1024;
    if (dir->tables[table_index]) /* Table is already assigned */
    {
        return &dir->tables[table_index]->pages[address%1024];
    }
    else if (create)
    {
        uint32_t tmp; /* Physical address of the page table */
        dir->tables[table_index] = kmalloc_ap(          \
            sizeof(struct page_table_t), &tmp);
        memset(dir->tables[table_index], 0, 0x1000);
        
        /* present, writable, available from ring 3 */
        dir->tables_physaddr[table_index] = tmp | 0x7;

        return &dir->tables[table_index]->pages[address%1024];
    }
    else
        return 0;
}

/* Clone a page table */
static struct page_table_t* clone_table(struct page_table_t* src,   \
                                        uint32_t phys_addr)
{
    /* Make a new page table */
    struct page_table_t* table = kmalloc_ap(        \
        sizeof(struct page_table_t), phys_addr);
    memset(table, 0, sizeof(struct page_directory_t));

    /* For all the entries in the table */
    int i;
    for (i = 0; i < 1024; i++)
    {
        /* Skip empty pages */
        if (!src->pages[i].frame)
            continue;

        /* Get a new frame */
        alloc_frame(&table->pages[i], 0, 0);

        /* Clone the flags */
        if (src->pages[i].present) table->pages[i].present = 1;
        if (src->pages[i].rw) table->pages[i].rw = 1;
        if (src->pages[i].user) table->pages[i].user = 1;
        if (src->pages[i].accessed) table->pages[i].accessed = 1;
        if (src->pages[i].dirty) table->pages[i].dirty = 1;

        /* Clone the data */
        memcpy(table->pages[i].frame*0x1000, src->pages[i].frame*0x1000, 0x1000);
    }
    return table;
}

/* Clone a page dir */
struct page_directory_t* clone_page_dir(struct page_directory_t* src)
{
    uint32_t phys_addr; /* Physical address of the page directory */

    /* Create a new page directory, get its phys. address */
    struct page_directory_t* dir = kmalloc_ap(          \
        sizeof(struct page_directory_t), &phys_addr);
    memset(dir, 0, sizeof(struct page_directory_t));

    /* Figure out the physical address of the new page tables */
    uint32_t offset = (uint32_t)dir->tables_physaddr - (uint32_t)dir;
    dir->physaddr = phys_addr + offset;
    
    /* Copy page tables */
    int i;
    /* First 32MB is for kernel -> link */
    
    for(i = 0; i < 1024; i++)
    {
        /* Skip empty page tables */
        if (!(src->tables[i]))
            continue;

        if (kernel_dir->tables[i] == src->tables[i])
        {            
            dir->tables[i] = src->tables[i];
            dir->tables_physaddr[i] = src->tables_physaddr[i];
        }
        else
        {            
            /* Copy the table if not from kernel */
            uint32_t phys;
            dir->tables[i] = clone_table(src->tables[i], &phys);
            dir->tables_physaddr[i] = phys | 0x07;
        }
    }
    return dir;
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

    kprintf("PAGE FAULT (0x%X at %p)\n", regs.err_code, faulty_address);
//    dump_heap(kheap);
    panic("PAGE FAULT");
}
