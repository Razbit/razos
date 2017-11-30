/* idt.c -- Interrupt Descriptor Table */

/* Copyright (c) 2015, 2017 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

/* Parts of this file are based on the work of Charlie Somerville.
 * The original file can be obtained here:
 * https://github.com/charliesome/radium
 *
 * Copyright (c) 2013-2015 Charlie Somerville
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#include <portio.h>
#include <sys/types.h>
#include <console.h>
#include "../gdt.h" /* GDT_KERNEL_CODE segment selector */

#include "idt.h"

extern void idt_init_asm(); /* isr.s */
extern void idt_load();     /* isr.s */

struct idt_entry_t
{
	uint16_t low_base;	/* Low 16 bits of the addr to jump to */
	uint16_t selector;	/* Kernel segment selector */
	uint8_t zero;		/* Always zero */
	uint8_t flags;		/* 0..3: 0xE; 4: 0; 5..6: PL; 7: Present? */
	uint16_t high_base; /* Upper 16 bits of the address */
} __attribute__((__packed__));

static struct idt_entry_t idt[256];

volatile struct
{
	uint16_t size;	   /* IDT size */
	struct idt_entry_t* base; /* Address of the first entry */
} __attribute__((__packed__)) idt_ptr;


/* Register an ISR handler. */
void register_isr(uint8_t int_no, void* handler)
{
	struct idt_entry_t entry;

	entry.low_base = (uint32_t)handler & 0xFFFF;
	entry.selector = GDT_KERNEL_CODE;
	entry.zero = 0;
	entry.flags = 0xEE; /* Is 32-interrupt gate, dpl=3, is present */
	entry.high_base = ((uint32_t)handler >> 16) & 0xFFFF;

	idt[int_no] = entry;
}

/* Remap irq's as interrupt vectors starting from 32 */
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

	register_isr(0, isr0);
	register_isr(1, isr1);
	register_isr(2, isr2);
	register_isr(3, isr3);
	register_isr(4, isr4);
	register_isr(5, isr5);
	register_isr(6, isr6);
	register_isr(7, isr7);
	register_isr(8, isr8);
	register_isr(9, isr9);
	register_isr(10, isr10);
	register_isr(11, isr11);
	register_isr(12, isr12);
	register_isr(13, isr13);
	register_isr(14, isr14);
	register_isr(16, isr16);
	register_isr(17, isr17);
	register_isr(18, isr18);
	register_isr(19, isr19);
	register_isr(20, isr20);
	register_isr(30, isr30);
	register_isr(32, isr32);
	register_isr(33, isr33);
	register_isr(39, isr39);

	idt_ptr.size = sizeof(idt) - 1;
	idt_ptr.base = idt;
	kprintf("Load idt (%u bytes) at 0x%x\n", idt_ptr.size, idt_ptr.base);
	idt_load(); /* isr.s */
}
