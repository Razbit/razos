/* This file is a part of the RazOS project
 *
 * descriptor_tables.h -- the x86 initializes descriptor tables (GDT, IDT)
 * and defines the default ISR and IRQ handlers
 *
 * Razbit 2014 */

#include "descriptor_tables.h"

#include "../lib/stdint.h"

#define NUM_GDTS 5

extern void gdt_flush(uint32_t); /* gdt.s */

static void init_gdt();
static void gdt_set(int32_t num, uint32_t base, uint32_t limit, \
                    uint8_t access_fl, uint8_t gran);

gdt_entry_t gdt_entries[NUM_GDTS];
gdt_ptr_t gdt_ptr;

/* Initialization: zero all ISRs, init GDT and IDT */
void init_descr_tables()
{
    init_gdt();
}

static void init_gdt()
{
    gdt_ptr.limit = (sizeof(gdt_entry_t) * NUM_GDTS) - 1;
    gdt_ptr.base = (uint32_t) &gdt_entries;

    gdt_set(0, 0, 0, 0, 0);                /* null segment */
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* kernel code */
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* kernel data */
    gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* user code */
    gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* user data */
}

static void gtd_set(int32_t num, uint32_t base, uint32_t limit, \
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
