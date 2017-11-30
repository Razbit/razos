/* task.s -- Assembly helpers for tasking
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

/* using cur_task from task.c
 * using sched_next from task.c
 * using task_fork_inner from task.c
 * using sched_cont from task.c
 * using get_page_dir_phys from paging.c
 */

.global sched_begin             /* sched.h*/
.global sched_switch            /* sched.h*/
.global task_fork               /* task.h*/

sched_begin:
    movl 12(%esp), %esi        /* envp*/
    movl 8(%esp), %edi         /* argv*/
    movl 4(%esp), %ecx         /* user stack end*/

    call sched_cont

    movl $0x23, %eax
    movl %eax, %ds
    movl %eax, %es
    movl %eax, %fs
    movl %eax, %gs

    movl $0x10000000, %edx      /* task entry point*/

    sti                         /* Now the PIT may call sched_switch*/
    sysexit                     /* Enter the user world*/

sched_switch:
    /*  save old task state*/
    pusha
    movl (cur_task), %eax
    fxsave (%eax)
    movl %esp, 0x200(%eax)
    pushl %ebx
    movl $sw.return, %ebx
    movl %ebx, 0x204(%eax)
    popl %ebx

    /*  fetch the next task*/
    call sched_next

    movl %eax, (cur_task)

    /*  get physical page dir address*/
    pushl 0x208(%eax)
    call get_page_dir_phys
    addl $4, %esp
    /*  eax now contains physical address of the new task's page dir*/
    /*  load to cr3*/
    movl %eax, %cr3

    /*  restore eax*/
    movl (cur_task), %eax

    /*  restore task state*/
    fxrstor (%eax)
    movl 0x200(%eax), %esp
    jmp *0x204(%eax)

sw.return:
    popa
    ret

task_fork:
    xor %eax, %eax

    pusha
    call task_fork_inner        /* create & set up the new task*/

    /* We save the return value so the parent that calls us gets
     * the right task_t* for the new child
     */
    movl %eax, 28(%esp)        /* EAX from pusha*/

    movl %esp, 0x200(%eax)
    pushl %ebx
    movl $tf.return, %ebx
    movl %ebx, 0x204(%eax)
    popl %ebx

tf.return:
    popa
    ret
