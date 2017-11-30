/* isr.s -- Interrupt Service Routines
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
 */

/* using idt_ptr from idt.c
 * using sched_switch from task.s
 * using sched_halted from task.c
 * using except from except.c
 * using except_err from except.c
 * using kb_handler from kb.c
 * using pagefault_handler from pagefault.c
 * using gpf_handler from gpf.c
 */

.section .text

/* Load new IDT. Called in idt.c, idt_init() */
.global idt_load
idt_load:
    lidt (idt_ptr)
    ret

.global isr0
isr0:
    pushl $0
    call except
    addl $4, %esp
    iret

.global isr1
isr1:
    pushl $1
    call except
    addl $4, %esp
    iret

.global isr2
isr2:
    pushl $2
    call except
    addl $4, %esp
    iret

.global isr3
isr3:
    pushl $3
    call except
    addl $4, %esp
    iret

.global isr4
isr4:
    pushl $4
    call except
    addl $4, %esp
    iret

.global isr5
isr5:
    pushl $5
    call except
    addl $4, %esp
    iret

.global isr6
isr6:
    pushl $6
    call except_err
    addl $4, %esp
    iret

.global isr7
isr7:
    pushl $7
    call except
    addl $4, %esp
    iret

.global isr8
isr8:
    pushl $8
    call except_err
    addl $4, %esp
    iret

.global isr9
isr9:
    pushl $9
    call except
    addl $4, %esp
    iret

.global isr10
isr10:
    pushl $10
    call except_err
    addl $4, %esp
    iret

.global isr11
isr11:
    pushl $11
    call except_err
    addl $4, %esp
    iret

.global isr12
isr12:
    pushl $12
    call except_err
    addl $4, %esp
    iret

.global isr16
isr16:
    pushl $16
    call except
    addl $4, %esp
    iret

.global isr17
isr17:
    pushl $17
    call except_err
    addl $4, %esp
    iret

.global isr18
isr18:
    pushl $18
    call except
    addl $4, %esp
    iret

.global isr19
isr19:
    pushl $19
    call except
    addl $4, %esp
    iret

.global isr20
isr20:
    pushl $20
    call except
    addl $4, %esp
    iret

.global isr30
isr30:
    pushl $30
    call except_err
    addl $4, %esp
    iret


    /*  General Protection Fault*/
.global isr13
isr13:

    /*  stack: [esp+4]: faulting eip*/
    /*         [esp]: error code*/
    pusha
    movl 36(%esp), %eax        /* eip*/
    pushl %eax
    movl 36(%esp), %eax         /* error*/
    push %eax

    call gpf_handler           /* gpf_handler(err, eip)*/
    addl $8, %esp
    popa
    addl $4, %esp
    iret

    /* Page fault */
.global isr14
isr14:
    movl %cr2, %eax
    pushl %eax

/* stack: [esp+8]: faulting eip
 *        [esp+4]: error code
 *        [esp]:   addr
 */

    call pagefault_handler      /* handler(addr, error, eip)*/
    addl $8, %esp
    iret

    /*  PIT*/
.global isr32
isr32:
    pusha
    /* ack */
    pushw %ax
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    popw %ax

    cmpl $1, (sched_halted)
    je .skip

    pushl %ebp
    pushl $0
    pushl $0
    movl %esp, %ebp

    call sched_switch

    addl $8, %esp
    popl %ebp

.skip:
    popa
    iret

    /*  Keyboard*/
.global isr33
isr33:
    pusha
    /* ack */
    pushw %ax
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    popw %ax

    call kb_handler
    popa
    iret

    /*  Spurious interrupt (or lpt1)*/
.global isr39
isr39:
    pusha
    pushw %ax                   /* save ax*/
    movb $0x0b, %al             /* cmd to read irq service reg (isr)*/
    outb %al, $0x20             /* command port of PIC1*/
    inb $0x20, %al              /* al = register contents*/
    test $0x80, %al             /* test if bit 7 is set*/
    popw %ax                    /* restore ax*/
    jz .spurious                /* jump if bit 7 is not set*/

    /*  the irq is not spurious, so ACK it and handle*/
    pushw %ax
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    popw %ax

.spurious:
    /*  should not be acknowledged, so just iret*/
    popa
    iret
