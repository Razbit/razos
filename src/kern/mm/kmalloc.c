/* This file is a part of the RazOS project
 *
 * kmalloc.c -- defines kernel-world malloc() family
 *
 * Razbit 2014 */

#include "kmalloc.h"
#include <stdint.h>
#include <stddef.h>

/* The end is defined in the linker script */
extern uint32_t end;
uint32_t placement_addr = (uint32_t)&end;

static uint32_t do_kmalloc(size_t size, uint8_t align, uint32_t* physaddr)
{
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

    return ret;
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

