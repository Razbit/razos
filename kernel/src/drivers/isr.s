;; isr.s -- Interrupt Service Routines

;; Copyright (c) 2015-2016 Eetu "Razbit" Pesonen
;;
;; This file is part of RazOS.
;;
;; RazOS is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License version 2
;; as published by the Free Software Foundation.
;;
;; RazOS is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with RazOS. If not, see <http://www.gnu.org/licenses/>.

;; Parts of this file are based on the work of Charlie Somerville.
;; The original file can be obtained here:
;; https://github.com/charliesome/radium
;;
;; Copyright (c) 2013-2015 Charlie Somerville
;;
;; MIT License
;;
;; Permission is hereby granted, free of charge, to any person
;; obtaining a copy of this software and associated documentation
;; files (the "Software"), to deal in the Software without
;; restriction, including without limitation the rights to use, copy,
;; modify, merge, publish, distribute, sublicense, and/or sell copies
;; of the Software, and to permit persons to whom the Software is
;; furnished to do so, subject to the following conditions:
;;
;; The above copyright notice and this permission notice shall be
;; included in all copies or substantial portions of the Software.
;;
;; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
;; EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
;; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
;; NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
;; BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
;; ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
;; CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;; SOFTWARE.

[GLOBAL idt_init_asm]
[GLOBAL idt_load]

    ;; idt.c: void register_isr(uint8_t int_no, void* handler)
[EXTERN register_isr]
[EXTERN idt_ptr]                ; idt.c
[EXTERN panic]                  ; panic.c

    ;; C-side interrupt handlers
[EXTERN sched_switch]           ; task.s
[EXTERN sched_halted]           ; task.c
[EXTERN kb_handler]             ; kb.c
[EXTERN pagefault_handler]      ; pagefault.c
[EXTERN gpf_handler]            ; gpf.c

    ;; Some quite heavy macro-magic
    ;; Have a hard stare at this and you'll figure it out

%macro REGISTER_ISR 1
    push isr_%1                 ; Push interrupt handler address
    push %1                     ; Push interrupt number
    call register_isr
    add esp, 8
%endmacro

%macro ACK_IRQ 0
    push ax
    mov al, 0x20
    out 0xa0, al
    out 0x20, al
    pop ax
%endmacro

%macro BEGIN_ISR 1
    jmp isr_%1.end
    isr_%1:
%endmacro

%macro END_ISR 1
    .end:
    REGISTER_ISR %1
%endmacro

%macro GEN_EXCEPTION 2
    BEGIN_ISR %1
        push .msg
        call panic
        .msg db %2, 0
    END_ISR %1
%endmacro

SECTION .text

    ;; Load new IDT. Called in idt.c, idt_init()
idt_load:
    lidt[idt_ptr]
    ret

    ;; Continue initialization of IDT. Called in idt.c, idt_init()
idt_init_asm:
    GEN_EXCEPTION 0, "int 0: divide by zero at 0x%x"
    GEN_EXCEPTION 1, "int 1: debug at 0x%x"
    GEN_EXCEPTION 2, "int 2: NMI at 0x%x"
    GEN_EXCEPTION 3, "int 3: breakpoint at 0x%x"
    GEN_EXCEPTION 4, "int 4: overflow at 0x%x"
    GEN_EXCEPTION 5, "int 5: bound range exceeded at 0x%x"
    GEN_EXCEPTION 6, "int 6: invalid opcode 0x%x at 0x%x"
    GEN_EXCEPTION 7, "int 7: device not available at 0x%x"
    GEN_EXCEPTION 8, "int 8: double fault 0x%x at 0x%x"
    GEN_EXCEPTION 9, "int 9: coprocessor segment overrun at 0x%x"
    GEN_EXCEPTION 10, "int 10: invalid TSS 0x%x at 0x%x"
    GEN_EXCEPTION 11, "int 11: segment not present 0x%x at 0x%x"
    GEN_EXCEPTION 12, "int 12: stack segment fault 0x%x at 0x%x"
    GEN_EXCEPTION 16, "int 16: x87 floating-point exception at 0x%x"
    GEN_EXCEPTION 17, "int 17: Alignment check 0x%x at 0x%x"
    GEN_EXCEPTION 18, "int 18: Machine check at 0x%x"
    GEN_EXCEPTION 19, "int 19: SIMD FP exception at 0x%x"
    GEN_EXCEPTION 20, "int 20: Virtualization at 0x%x"
    GEN_EXCEPTION 30, "int 30: Security exception 0x%x at 0x%x"

    ;; General Protection Fault
BEGIN_ISR 13
    ;; stack: [esp+4]: faulting eip
    ;;        [esp]: error code
    pusha
    mov eax, [esp+8*4+4]        ; eip
    push eax
    mov eax, [esp+8*4+4]        ; error
    push eax

    call gpf_handler            ; gpf_handler(err, eip)
    add esp, 8
    popa
    add esp, 4
    iret
END_ISR 13

    ;; Page fault
BEGIN_ISR 14
    mov eax, cr2
    push eax

    ;; stack: [esp+8]: faulting eip
    ;;        [esp+4]: error code
    ;;        [esp]:   addr

    call pagefault_handler      ; handler(addr, error, eip)
    add esp, 8
    iret
END_ISR 14

    ;; PIT
BEGIN_ISR 32
    pusha
    ACK_IRQ

    cmp [sched_halted], dword 1
    je .skip

    push ebp
    push dword 0
    push dword 0
    mov ebp, esp

    call sched_switch

    add esp, 8
    pop ebp

.skip:
    popa
    iret
END_ISR 32

    ;; Keyboard
BEGIN_ISR 33
    pusha
    ACK_IRQ

    call kb_handler
    popa
    iret
END_ISR 33

    ;; Spurious interrupt (or lpt1)
BEGIN_ISR 39
    pusha
    push ax                     ; save ax
    mov al, 0x0b                ; cmd to read irq service reg (isr)
    out 0x20, al                ; command port of PIC1
    in al, 0x20                 ; al = register contents
    test al, 0x80               ; test if bit 7 is set
    pop ax                      ; restore ax
    jz .spurious                ; jump if bit 7 is not set

    ;; the irq is not spurious, so ACK it and handle
    ACK_IRQ

.spurious:
    ;; should not be acknowledged, so just iret
    popa
    iret
END_ISR 39

ret
