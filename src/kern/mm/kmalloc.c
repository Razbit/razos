/* This file is a part of the RazOS project
 *
 * kmalloc.c -- defines kernel-world malloc() family
 *
 * Razbit 2014 */

#include "kmalloc.h"
#include "../kio.h"
#include "heap.h"
#include "paging.h"

#include <stdint.h>
#include <stddef.h>

/* The end is defined in the linker script */
extern uint32_t end;
uint32_t placement_addr = (uint32_t)&end;

/* The kernel heap resides here */
struct heap_t* kheap = 0;

/* Paging.c */
extern struct page_directory_t* kernel_dir;

static uint32_t do_kmalloc(size_t size, uint8_t align, uint32_t* physaddr)
{
    if (kheap != 0) /* We are heaping already */
    {
        void* addr = alloc(size, kheap, align);
        if (physaddr != 0)
        {
            struct page_t *pg = get_page(addr, 0, kernel_dir);
            *physaddr = pg->frame*0x1000 + (uint32_t)addr&0xFFF;
        }
        //kprintf("kmalloc: %d %d %p\n", size, align, addr);
        return (uint32_t)addr;
    }
    else
    {        
        /* 8-byte alignment */
        placement_addr &= 0xFFFFFFF8;
        placement_addr += 0x8;
    
        if (align && (placement_addr & 0xFFFFF000)) /* Not aligned already */
        {
            /* Align to 4KB page boundary */
            placement_addr &= 0xFFFFF000;
            placement_addr += 0x1000;
        }
        if (physaddr)
            *physaddr = placement_addr;
    
        uint32_t ret = placement_addr;
        placement_addr += size;

        kprintf("kmalloc: %d %d %p\n", size, align, ret);

        return ret;
    }
}

/* Page aligned */
void* kmalloc_a(size_t size)
{
    return (void*)do_kmalloc(size, 1, NULL);
}

/* Returns physical address */
void* kmalloc_p(size_t size, uint32_t* physaddr)
{
    return (void*)do_kmalloc(size, 0, physaddr);
}

/* Page aligned and returns physical address */
void* kmalloc_ap(size_t size, uint32_t* physaddr)
{
    return (void*)do_kmalloc(size, 1, physaddr);
}

/* Normal malloc() */
void* kmalloc(size_t size)
{
    return (void*)do_kmalloc(size, 0, NULL);
}

