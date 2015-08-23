/* This file is a part of the RazOS project
 *
 * gdt.h -- the x86 GDT
 *
 * Razbit 2014 */

#include "gdt.h"
#include "kio.h"

#include <sys/types.h>

extern void gdt_flush(uint32_t); /* gdt.s */
static void gdt_set(int32_t num, uint32_t base, uint32_t limit, \
                    uint8_t access_fl, uint8_t gran);

struct gdt_entry_t gdt_entries[5];
struct gdt_ptr_t gdt_ptr;

void init_gdt()
{   
    gdt_ptr.limit = sizeof(struct gdt_entry_t) * 5 - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    kprintf("Initializing GDT at 0x%p\n",\
            &gdt_entries);
    
    gdt_set(0, 0, 0, 0, 0);                /* null segment */
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* kernel code */
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* kernel data */
    gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* user code */
    gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* user data */

    gdt_flush((uint32_t)&gdt_ptr);
}

static void gdt_set(int32_t num, uint32_t base, uint32_t limit, \
                    uint8_t access_fl, uint8_t gran)
{
    gdt_entries[num].low_base = (base & 0xFFFF);
    gdt_entries[num].mid_base = (base >> 16) & 0xFF;
    gdt_entries[num].high_base = (base >> 24) & 0xFF;

    gdt_entries[num].low_lim = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access_fl = access_fl;
}
