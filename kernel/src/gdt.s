/* gdt.s -- Global Descriptor Table assembly part
 *
 * Copyright (c) 2015 - 2017 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>.
 */

    /*  Using gdt_ptr from gdt.c*/
.global gdt_reload             /* in gdt.h as void gdt_reload()*/

.section .text

gdt_reload:
    cli
    lgdt (gdt_ptr)              /* Load GDT table specified by gdtr*/

    /*  Enable protected mode*/
    movl %cr0, %eax
    orb $1, %al
    movl %eax, %cr0

    /*  Load CS using a far jump*/
    jmp $0x08, $.flush_cs
.flush_cs:
    movl $0x10, %eax
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs
    movl %eax, %ss
    ret
