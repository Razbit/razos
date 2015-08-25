/* This file is a part of the RazOS project
 *
 * detect.h -- memory detection
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <asm/multiboot.h>
#include "detect.h"

#include "../kio.h" /* kprintf() */

/* Print the memory map */
void dump_memdetect(struct multiboot_info* mb)
{
    if (mb->flags & MULTIBOOT_FLAG_MEM)
    {
        kprintf("Lower memory: %u KiB\n", mb->mem_lower);
        kprintf("Upper memory: %u KiB\n", mb->mem_upper);
    }
}

/* Return size of available upper memory */
size_t get_avail_mem(struct multiboot_info* mb)
{
    size_t mem = 0x8000000; /* Default to 128 MB */
    if (mb->flags & MULTIBOOT_FLAG_MEM)
    {
        mem = mb->mem_upper;
        mem *= 0x400;
        /* Page-align (4K pages) */
        mem &= 0xFFFFF000;
    }

    return mem;
}
