    ;; This file is a part of the RazOS project
    ;;
    ;; isr.s -- Interrupt Service Routines
    ;;
    ;; Razbit 2015 (based on Charlie Somerville's Radium)

[GLOBAL idt_init_asm]
[GLOBAL idt_load]

    ;; idt.c: void interrupt_register_isr(uint8_t int_no, void* handler)
[EXTERN interrupt_register_isr]
[EXTERN idt_ptr]                ; idt.c
[EXTERN panic]                  ; panic.c

    ;; C-side interrupt handlers
[EXTERN sched_switch]           ; task.c
[EXTERN kb_handler]             ; kb.c

    ;; Some quite heavy macro-magic
    ;; Have a hard stare at this and you'll figure it out

%macro REGISTER_ISR 1
    push isr_%1                 ; Push interrupt handler address
    push %1                     ; Push interrupt number
    call interrupt_register_isr ; in idt.c
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
    GEN_EXCEPTION 0, "int 0: divide by zero"
    GEN_EXCEPTION 1, "int 1: debug"
    GEN_EXCEPTION 2, "int 2: NMI"
    GEN_EXCEPTION 3, "int 3: breakpoint"
    GEN_EXCEPTION 4, "int 4: overflow"
    GEN_EXCEPTION 5, "int 5: bound range exceeded"
    GEN_EXCEPTION 6, "int 6: invalid opcode"
    GEN_EXCEPTION 7, "int 7: device not available"
    GEN_EXCEPTION 8, "int 8: double fault"
    GEN_EXCEPTION 9, "int 9: coprocessor segment overrun"
    GEN_EXCEPTION 10, "int 10: invalid TSS"
    GEN_EXCEPTION 11, "int 11: segment not present"
    GEN_EXCEPTION 12, "int 12: stack segment fault"
    GEN_EXCEPTION 13, "int 13: general protection fault"

    ;; page fault
BEGIN_ISR 14
    mov eax, cr2
    push eax
    push .msg
    call panic
    iret

    .msg db "page fault at 0x%x, error code %x", 0
END_ISR 14

    ;; PIT
BEGIN_ISR 32
    ACK_IRQ

    ;push ebp
    ;push dword 0
    ;push dword 0
    ;mov ebp, esp

    ;call sched_switch

    ;add esp, 8
    ;pop ebp

    iret
END_ISR 32

    ;; Keyboard
BEGIN_ISR 33
    ACK_IRQ

    call kb_handler

    iret
END_ISR 33

ret