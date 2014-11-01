/* This file is a part of the RazOS project
 *
 * descriptor_tables.h -- the x86 descriptor tables (GDT, IDT)
 *
 * Razbit 2014 */

#ifndef DESCRIPTOR_TABLES_H
#define DESCRIPTOR_TABLES_H

#include "../lib/stdint.h"

/* Contains a GDT entry */
struct gdt_entry_t
{
    uint16_t low_lim;   /* Low 16 bits of the segment limit */
    uint16_t low_base;  /* Low 16 bits of the base address */
    uint8_t mid_base;   /* Base address bits 16..23 */
    uint8_t access_fl;  /* 0..3: Type (code/data); 4: Descriptor type;
                         * 5..6: Privilege (ring); 7: Present (y/n) */
    uint8_t granularity; /* 0..3: Segment limit bits 16..19; 4..5: zero;
                         * 6: Operand size (16/32) 7: Byte/kbyte */
    uint8_t high_base;  /* Base address bit 24..31 */
}__attribute__((__packed__)); /* Don't change alignment */

/* Used to tell the CPU where the GDT is */
struct gdt_ptr_t
{
    uint16_t limit; /* Max size of the GDT (minus 1) */
    uint32_t base;  /* Address of the first gdt_entry struct */
}__attribute__((__packed__));

struct idt_entry_t
{
    uint16_t low_base;  /* Low 16 bits of the addr to jump to */
    uint16_t selector;  /* Kernel segment selector */
    uint8_t zero;       /* Always zero */
    uint8_t flags;      /* 0..4: always 0x0E; 5..6: Ring; 7: Present? */ 
    uint16_t high_base; /* Upper 16 bits of the address */
}__attribute__((__packed__));

struct idt_ptr_t
{
    uint16_t limit; /* Max size of the IDT */
    uint32_t base;  /* Address of the first element */
}__attribute__((__packed__));

void init_descr_tables();

/* The 32 must-have Interrupt Service Routines */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

#endif /* DESCRIPTOR_TABLES_H */

