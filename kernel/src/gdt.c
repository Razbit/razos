/* gdt.c -- Global Descriptor Tables */

/* Copyright (c) 2015 Eetu "Razbit" Pesonen
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

#include <sys/types.h>
#include <asm/x86.h>
#include <panic.h>
#include <console.h>

#include "gdt.h"

struct gdt_entry_t
{
	uint16_t low_lim;	 /* Low 16 bits of the segment limit */
	uint16_t low_base;	 /* Low 16 bits of the base address */
	uint8_t mid_base;	 /* Base address bits 16..23 */
	uint8_t access;		 /* 0..3: Type (code/data); 4: Descriptor type;
			              * 5..6: Privilege (ring); 7: Present (y/n) */
	uint8_t granularity; /* 0..3: Segment limit bits 16..19; 4..5: zero;
	                      * 6: Operand size (16/32) 7: Byte/kbyte */
	uint8_t high_base;	 /* Base address bits 24..31 */
} __attribute__((__packed__));

/* The GDT table entries */
static struct gdt_entry_t gdt[6];

volatile struct
{
	uint16_t size;			  /* Max size of the GDT (minus 1) */
	struct gdt_entry_t* base; /* Address of the first GDT entry */
} __attribute__((__packed__)) gdt_ptr;

/* Set a GDT table entry */
static void do_gdt_set_entry(uint32_t sel, uint32_t base,	\
                             uint32_t limit, uint8_t access)
{
	if (sel >= sizeof(gdt))
	{
		panic("GDT overflow");
	}

	uint8_t flags = 1 << 6; /* 32-bit operand */

	if (limit >= 1 << 20) /* If we are working with > 1MiB segments */
	{
		flags |= 1 << 7; /* Use 1 KiB granularity. Otherwise 1 B */
		limit /= 4096;
	}

	struct gdt_entry_t entry;
	entry.low_base = base & 0xFFFF;
	entry.mid_base = (base >> 16) & 0xFF;
	entry.high_base = (base >> 24) & 0xFF;
	entry.low_lim = limit & 0xFFFF;
	entry.granularity = ((limit >> 16) & 0x0F) | flags;
	entry.access = access;

	gdt[sel/sizeof(struct gdt_entry_t)] = entry;
}

void gdt_set_entry(uint32_t sel, uint32_t base, uint32_t limit, \
                   uint32_t priv, uint32_t type)
{
	uint32_t access = 1 << 7;	 /* present */
	access |= ((priv & 3) << 5); /* privilege */
	access |= (1 << 4);			 /* dunno lol */
	access |= ((type & 1) << 3); /* code or data? */
	access |= (1 << 1);			 /* data RW, code R */

	do_gdt_set_entry(sel, base, limit, access);
}

void gdt_set_tss(uint32_t sel, uint32_t base, uint32_t limit)
{
	/* Access byte 0x89: present|executable|accessed */
	do_gdt_set_entry(sel, base, limit, 0x89);
}

void gdt_init()
{
	gdt_ptr.size = sizeof(gdt) - 1;
	gdt_ptr.base = gdt;

	gdt_set_entry(GDT_KERNEL_CODE, 0, 0xFFFFFFFF, GDT_KERNEL, GDT_CODE);
	gdt_set_entry(GDT_KERNEL_DATA, 0, 0xFFFFFFFF, GDT_KERNEL, GDT_DATA);
	gdt_set_entry(GDT_USER_CODE, 0, 0xFFFFFFFF, GDT_USER, GDT_CODE);
	gdt_set_entry(GDT_USER_DATA, 0, 0xFFFFFFFF, GDT_USER, GDT_DATA);

	gdt_reload();
}
