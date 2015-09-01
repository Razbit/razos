/* This file is a part of the RazOS project
 *
 * kmalloc.c -- defines kernel-world malloc() family
 *
 * Razbit 2014 */

#include "kmalloc.h"
#include "../kio.h"
#include "heap.h"
#include "kheap.h"
#include "paging.h"

#include <sys/types.h>

/* #define _DEBUG */

/* The end is defined in the linker script */
extern uint32_t end;
uint32_t placement_addr = (uint32_t)&end;

/* The kernel heap resides here */
struct heap_t* kheap = 0;

/* Paging.c */
extern struct page_directory_t* kernel_dir;
extern struct page_directory_t* cur_dir;

static uint32_t kmalloc_int(size_t size, uint8_t align, uint32_t* physaddr)
{
    if (kheap != 0) /* We are heaping already */
    {
        void* addr = do_kmalloc(size, align);
        kprintf("KMALLOC: allocated 0x%p (%u B) at 0x%p\n", size, size, addr);
        if (physaddr != 0)
        {
            *physaddr = (uint32_t)addr;
        }
        
        #ifdef _DEBUG
        kprintf("kmalloc: heap: sz: %p al: %d ad: %p\n", size, align, addr);
        #endif

        return (uint32_t)addr;
    }
    else
    {        
        /* 16-byte alignment */
        if (placement_addr & 0x0F)
        {
            placement_addr &= 0xFFFFFFF0;
            placement_addr += 0x10;
        }
    
        if (align && (placement_addr & 0x00000FFF)) /* Not aligned already */
        {
            /* Align to 4KB page boundary */
            placement_addr &= 0xFFFFF000;
            placement_addr += 0x1000;
        }
        if (physaddr)
            *physaddr = placement_addr;
    
        uint32_t ret = placement_addr;
        placement_addr += size;

        #ifdef _DEBUG
        kprintf("kmalloc: %p %d %p %p\n", size, align, ret, placement_addr);
        #endif
        
        return ret;
    }
}

/* Page aligned */
void* kmalloc_a(size_t size)
{
    return (void*)kmalloc_int(size, 1, NULL);
}

/* Returns physical address */
void* kmalloc_p(size_t size, uint32_t* physaddr)
{
    return (void*)kmalloc_int(size, 0, physaddr);
}

/* Page aligned and returns physical address */
void* kmalloc_ap(size_t size, uint32_t* physaddr)
{
    return (void*)kmalloc_int(size, 1, physaddr);
}

/* Normal malloc() */
void* kmalloc(size_t size)
{
    return (void*)kmalloc_int(size, 0, NULL);
}

/* Normal free() */
void kfree(void* ptr)
{
    do_kfree(ptr);
}

