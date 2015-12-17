/* This file is a part of the RazOS project
 *
 * idt.c -- Interrupt Descriptor Table
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#include <portio.h>
#include <sys/types.h>
#include <console.h>
#include "../gdt.h" /* GDT_KERNEL_CODE segment selector */

#include "idt.h"

extern void idt_init_asm(); /* isr.s */
extern void idt_load(); /* isr.s */

struct idt_entry_t
{
	uint16_t low_base;	/* Low 16 bits of the addr to jump to */
	uint16_t selector;	/* Kernel segment selector */
	uint8_t zero;		/* Always zero */
	uint8_t flags;		/* 0..3: always 0xE; 4: 0; 5..6: Ring; 7: Present? */
	uint16_t high_base; /* Upper 16 bits of the address */
} __attribute__((__packed__));

static struct idt_entry_t idt[256];

volatile struct
{
	uint16_t size;	   /* IDT size */
	struct idt_entry_t* base; /* Address of the first entry */
} __attribute__((__packed__)) idt_ptr;


/* Register an ISR handler. Called in isr.s */
void interrupt_register_isr(uint8_t int_no, void* handler)
{
	struct idt_entry_t entry;

	entry.low_base = (uint32_t)handler & 0xFFFF;
	entry.selector = GDT_KERNEL_CODE;
	entry.zero = 0;
	entry.flags = 0xEE; /* Is 32-interrupt gate, dpl=3, is present */
	entry.high_base = ((uint32_t)handler >> 16) & 0xFFFF;

	idt[int_no] = entry;
}

static void irq_remap()
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

void idt_init()
{
	irq_remap();
	idt_init_asm(); /* isr.s */

	idt_ptr.size = sizeof(idt) - 1;
	idt_ptr.base = idt;
	kprintf("Load idt (%u bytes) at 0x%x\n", idt_ptr.size, idt_ptr.base);
	idt_load(); /* isr.s */
}
