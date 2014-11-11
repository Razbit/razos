/* This file is a part of the RazOS project
 *
 * idt.h -- the x86 IDT
 *
 * Razbit 2014 */

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void init_idt();
void idt_set(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

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

#endif /* IDT_H */
