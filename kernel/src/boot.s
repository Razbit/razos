/* boot.s -- Kernel start location. Also defines multiboot header.
 *
 * Copyright (c) 2014 - 2017 Eetu "Razbit" Pesonen
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
 *
 *
 * Parts of this file are based on the work of Charlie Somerville.
 * The original file can be obtained here:
 * https://github.com/charliesome/radium
 *
 * Copyright (c) 2013-2015 Charlie Somerville
 */

.set MBOOT_PAGE_ALIGN,   1<<0   /* Load kernel and modules on a page boundary*/
.set MBOOT_MEM_INFO,     1<<1   /* Provide the kernel with memory info*/
.set MBOOT_HDR_MAGIC,    0x1BADB002
.set MBOOT_HDR_FLAGS,    MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
.set MBOOT_CHECKSUM,     -(MBOOT_HDR_MAGIC + MBOOT_HDR_FLAGS)


.section .multiboot
    .align 4
mboot:
    .long MBOOT_HDR_MAGIC       /* GRUB will search for this value*/
    .long MBOOT_HDR_FLAGS       /* How GRUB should load files/settings*/
    .long MBOOT_CHECKSUM        /* Check the above values are correct*/

.section .text

.global start                   /* Kernel entry point*/
.type start, @function

start:
    movl $stack_end, %esp       /* Setup the stack*/
    pushl $0
    pushl $0
    movl %esp, %ebp

    pushl %esp                  /* Push stack location*/
    pushl %ebx                  /* Push multiboot header location*/

    fninit                      /* Init the FPU*/
    movl %cr0, %eax
    orl $0x20, %eax              /* FPU NE bit*/
    movl %eax, %cr0

    cli                         /* Disable interrupts.*/
    call kmain                  /* C entry*/

    cli                         /* Go to infinite loop if kmain() returns*/
    jmp .hang
.hang:
    jmp .hang

.section .bss
    .align 16
stack_start:
    .skip 65535                 /* 64 KB of stack space*/
stack_end:

.global end_of_image            /* End of kernel image*/

.section .end_of_image
end_of_image:
