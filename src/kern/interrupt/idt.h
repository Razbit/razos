/* This file is a part of the RazOS project
 *
 * idt.h -- the x86 IDT; tells cpu where to find interrupt handlers
 *
 * Razbit 2014 */

#ifndef IDT_H
#define IDT_H

#include <sys/types.h>

/* Initialize the interrupt system */
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
extern void isr0(); /* Division by zero */
extern void isr1(); /* Debug exception */
extern void isr2(); /* NMI */
extern void isr3(); /* Breakpoint */
extern void isr4(); /* Into detected overflow */
extern void isr5(); /* Out of bounds */
extern void isr6(); /* Invalid opcode */
extern void isr7(); /* No coprocessor */
extern void isr8(); /* Double fault */
extern void isr9(); /* Coprocessor segment overrun */
extern void isr10(); /* Bad TSS */
extern void isr11(); /* Segment not present */
extern void isr12(); /* Stack fault */
extern void isr13(); /* General protection fault */
extern void isr14(); /* Page fault */
extern void isr15(); /* Unknown interrupt */
extern void isr16(); /* Coprocessor fault */
extern void isr17(); /* Alignment check */
extern void isr18(); /* Machine check */
extern void isr19(); /* Reserved from hereon */
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
