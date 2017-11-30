/* syscall.s -- syscall assembly helpers
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
 *
 *
 * Parts of this file are based on the work of Charlie Somerville.
 * The original file can be obtained here:
 * https://github.com/charliesome/radium
 *
 * Copyright (c) 2013-2015 Charlie Somerville
 */

/* using syscall_dispatch from syscall.c*/
.global syscall_init           /* syscall.h*/

syscall_init:
    xor %edx, %edx

    /* set up the MSR for sysenter/sysexit */
    /* IA32_SYSENTER_CS */
    movl $0x0174, %ecx
    movl $0x08, %eax
    wrmsr

    /* IA32_SYSENTER_ESP */
    movl $0x0175, %ecx
    movl $0x0FFFFFF0, %eax      /* sysenter stack is syscall stack*/
    wrmsr

    /* IA32_SYSENTER_EIP */
    movl $0x0176, %ecx
    movl $syscall_entry, %eax    /* We jump there upon syscall*/

    wrmsr

    ret

syscall_entry:
    pusha

    /*  create fake stack frame*/
    pushl $0
    pushl $0
    movl %esp, %ebp

    leal 8(%esp), %eax          /* last element of pusha at [esp+8]*/
    pushl %eax

    call syscall_dispatch       /* syscall.c*/

    addl $12, %esp               /* clean up the stack*/

    popa

    sti
    sysexit                     /* go to PL=3, perform_syscall.ret in crt0.s*/
