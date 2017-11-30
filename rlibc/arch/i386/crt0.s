/* crt0.s -- User side of syscall interface, program startup
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
 * but WITHOUT ANY WARRANTY without even the implied warranty of
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

.global _start
.global __syscall0
.global __syscall1
.global __syscall2
.global __syscall3
.global exit

    /*  using main, do_exit, init_rlibc, _init, _fini*/

.section .start

    /*  Entry of the user program.*/
_start:
    /* The execve() -syscall stores contents of argv, envp on the stack:
     * <envp contents><envp><argv contents><argv><argc>
     * pointer to envp is in esi, pointer to argv in edi
     */

    popl %eax                   /* argc in eax*/

    pushl %esi                  /* push envp*/
    pushl %edi                  /* push argv*/
    pushl %eax                  /* push argc*/

    /*  Prepare signals, memory allocation, stdio, errno etc.*/
    /*  In setup_rlibc.c*/
    call init_rlibc

    /*  Global constructors (crti.s)*/
    call _init

    call main
    pushl %eax

    call exit

.section .text
exit:
    movl 4(%esp), %ebx

    call _fini
    pushl %ebx

    call do_exit


    /*  Syscall interface */

    /*  No parameters, syscall num on stack*/
    /*  In razos.h as uint32_t __syscall0(num)*/
__syscall0:
    movl 4(%esp), %eax

    pushl %ecx
    pushl %edx
    movl %esp, %ecx              /* sysenter needs ecx, edx*/
    movl $sc0.ret, %edx

    sysenter

sc0.ret:
    popl %edx
    popl %ecx

    ret

    /*  One parameter and syscall number on stack*/
    /*  In razos.h as uint32_t __syscall1(num, arg1)*/
__syscall1:
    pushl %ebx
    movl 12(%esp), %ebx
    movl 8(%esp), %eax

    pushl %ecx
    pushl %edx
    movl %esp, %ecx              /* sysenter needs ecx, edx*/
    movl $sc1.ret, %edx

    sysenter

sc1.ret:
    popl %edx
    popl %ecx

    popl %ebx
    ret

    /*  Two parameters and syscall number on stack*/
    /*  In razos.h as uint32_t __syscall2(num, arg1, arg2)*/
__syscall2:
    pushl %ebx
    pushl %edi
    movl 20(%esp), %edi
    movl 16(%esp), %ebx
    movl 12(%esp), %eax

    pushl %ecx
    pushl %edx
    movl %esp, %ecx              /* sysenter needs ecx, edx*/
    movl $sc2.ret, %edx

    sysenter

sc2.ret:
    popl %edx
    popl %ecx

    popl %edi
    popl %ebx
    ret

    /*  Three parameters and syscall number on stack*/
    /*  In razos.h as uint32_t __syscall3(num, arg1, arg2, arg3)*/
__syscall3:
    pushl %ebx
    pushl %edi
    pushl %esi
    movl 28(%esp), %esi
    movl 24(%esp), %edi
    movl 20(%esp), %ebx
    movl 16(%esp), %eax

    pushl %ecx
    pushl %edx
    movl %esp, %ecx              /* sysenter needs ecx, edx*/
    movl $sc3.ret, %edx

    sysenter

sc3.ret:
    popl %edx
    popl %ecx

    popl %esi
    popl %edi
    popl %ebx
    ret
