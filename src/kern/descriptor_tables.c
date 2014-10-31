/* This file is a part of the RazOS project
 *
 * descriptor_tables.h -- the x86 initializes descriptor tables (GDT, IDT)
 * and defines the default ISR and IRQ handlers
 *
 * Razbit 2014 */

#include "descriptor_tables.h"

#include "../lib/stdint.h"

#define NUM_GDTS 5
#define NUM_IDTS 256

extern void gdt_flush(uint32_t); /* gdt.s */
extern void idt_flush(uint32_t); /* gdt.s */

static void init_gdt();
static void gdt_set(int32_t num, uint32_t base, uint32_t limit, \
                    uint8_t access_fl, uint8_t gran);

static void init_idt();
static void idt_set(uint8_t num, uint32_t base, uint16_t selector, \
                    uint8_t flags);

struct gdt_entry_t gdt_entries[NUM_GDTS];
struct gdt_ptr_t gdt_ptr;

struct idt_entry_t idt_entries[NUM_IDTS];
struct idt_ptr_t idt_ptr;

/* Initialization: zero all ISRs, init GDT and IDT */
void init_descr_tables()
{
    init_gdt();
    init_idt();
}

static void init_gdt()
{
    gdt_ptr.limit = sizeof(struct gdt_entry_t) * NUM_GDTS - 1;
    gdt_ptr.base = (uint32_t) &gdt_entries;

    gdt_set(0, 0, 0, 0, 0);                /* null segment */
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* kernel code */
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* kernel data */
    gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* user code */
    gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* user data */
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

static void init_idt()
{
    idt_ptr.limit = sizeof(struct idt_entry_t) * 256 - 1;
    idt_ptr.base = (uint32_t) &idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry_t) * 256);

    idt_set(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_set(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_set(8, (uint32_t)isr8, 0x08, 0x8E);
    idt_set(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_set(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set(22, (uint32_t)isr22, 0x08, 0x8E);
    idt_set(23, (uint32_t)isr23, 0x08, 0x8E);
    idt_set(24, (uint32_t)isr24, 0x08, 0x8E);
    idt_set(25, (uint32_t)isr25, 0x08, 0x8E);
    idt_set(26, (uint32_t)isr26, 0x08, 0x8E);
    idt_set(27, (uint32_t)isr27, 0x08, 0x8E);
    idt_set(28, (uint32_t)isr28, 0x08, 0x8E);
    idt_set(29, (uint32_t)isr29, 0x08, 0x8E);
    idt_set(30, (uint32_t)isr30, 0x08, 0x8E);
    idt_set(31, (uint32_t)isr31, 0x08, 0x8E);

    idt_flush((uint32_t)&idt_ptr);
}

static void idt_set(uint8_t num, uint32_t base, uint16_t selector, \
                    uint8_t flags)
{
    idt_entries[num].low_base = base & 0xFFFF;
    idt_entries[num].high_base = (base >> 16) & 0xFFFF;

    idt_entries[num].selector = selector;
    idt_entries[num].zero = 0;
    
    idt_entries[num].flags = flags;

    /* Uncomment when we start using the user mode
     * Sets privilege to 3 */
    
    /* idt_entries[num].flags |= 0x60; */
}
