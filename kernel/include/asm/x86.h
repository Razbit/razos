/* x86.h -- x86 architecture-specific thingies */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
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

#ifndef X86_H
#define X86_H

#include <sys/types.h>

/* Control registers */
/* CR0: operating mode, CPU states */
#define CR0_PG (1 << 31) /* Paging */
#define CR0_CD (1 << 30) /* Disable caching */
#define CR0_NW (1 << 29) /* When clear, enable write-through */
#define CR0_AM (1 << 18) /* Alignment check enabled */
#define CR0_WP (1 << 16) /* Supervisor can't write to read-only pages */
#define CR0_NE (1 << 5)  /* x87 FPU error reporting mode */
#define CR0_ET (1 << 4)  /* 387dx support (386, 486) */
#define CR0_TS (1 << 3)  /* x87/mmx/sse context saving delayed */
#define CR0_EM (1 << 2)  /* If set, no FPU available */
#define CR0_MP (1 << 1)  /* Controls interaction of WAIT with CR0_TS */
#define CR0_PE (1 << 0)  /* Enable protected mode */

/* CR3: 31..12: page directory base */
#define CR3_PCD (1 << 4) /* Page-directory cache-disable bit */
#define CR3_PWT (1 << 3) /* Page-directory write-through bit */

/* CR4: architectural extensions, indicate operating system
 * support for specific processor capabilities */
#define CR4_OSFXSR (1 << 9) /* Support for FXSAVE, FXRSTOR */
#define CR4_PGE (1 << 7)    /* Enable global pages */
#define CR4_MCE (1 << 6)    /* Enable machine-check exception */
#define CR4_PAE (1 << 5)    /* Enables PAE */
#define CR4_PSE (1 << 4)    /* Enables 4 MiB pages when set */
#define CR4_DE  (1 << 3)    /* Debugging extensions */
#define CR4_TSD (1 << 2)    /* Restrict RDTSC to PL 0 */
#define CR4_PVI (1 << 1)    /* Protected-Mode Virtual Interrupts */
#define CR4_VME (1 << 0)    /* Enable interrupt/exception handling in
                             * virtual-8086 mode */

void set_cr0(uint32_t flag);
void set_cr3(uint32_t flag);
void set_cr4(uint32_t flag);

void clear_cr0(uint32_t flag);
void clear_cr3(uint32_t flag);
void clear_cr4(uint32_t flag);

uint32_t get_cr0();
uint32_t get_cr3();
uint32_t get_cr4();

#define check_cr0(f) ((get_cr0() & f) ? 1 : 0)
#define check_cr3(f) ((get_cr3() & f) ? 1 : 0)
#define check_cr4(f) ((get_cr4() & f) ? 1 : 0)

/* Task State Segment */
struct tss_t
{
	uint16_t link;   /* 0x00 */
	uint16_t _res_1;
	uint32_t esp0;   /* 0x04 */
	uint16_t ss0;    /* 0x08 */
	uint16_t _res_2;
	uint32_t esp1;   /* 0x0C */
	uint16_t ss1;    /* 0x10 */
	uint16_t _res_3;
	uint32_t esp2;   /* 0x14 */
	uint16_t ss2;    /* 0x18 */
	uint16_t _res_4;
	uint32_t cr3;    /* 0x1C */
	uint32_t eip;    /* 0x20 */
	uint32_t eflags; /* 0x24 */
	uint32_t eax;    /* 0x28 */
	uint32_t ecx;    /* 0x2C */
	uint32_t edx;    /* 0x30 */
	uint32_t ebx;    /* 0x34 */
	uint32_t esp;    /* 0x38 */
	uint32_t ebp;    /* 0x3C */
	uint32_t esi;    /* 0x40 */
	uint32_t edi;    /* 0x44 */
	uint16_t es;     /* 0x48 */
	uint16_t _res_5;
	uint16_t cs;     /* 0x4C */
	uint16_t _res_6;
	uint16_t ss;     /* 0x50 */
	uint16_t _res_7;
	uint16_t ds;     /* 0x54 */
	uint16_t _res_8;
	uint16_t fs;     /* 0x58 */
	uint16_t _res_9;
	uint16_t gs;	 /* 0x5C */
	uint16_t _res_10;
	uint16_t ldtr;   /* 0x60 */
	uint16_t _res_11;
	uint16_t _res_12;
	uint16_t iopb;	 /* 0x66 */
}__attribute__((__packed__));

#endif /* X86_H */
