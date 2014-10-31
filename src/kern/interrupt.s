	    ;; This file is a part of the RazOS project
	    ;;
	    ;; interrupt.s -- ASM side of interrupt handling
	    ;;
	    ;; Razbit 2014

        ;; We dont receive any info on the interrupt when it fires, so we
        ;; need a different handler for each of them. We want keep clear of
        ;; duplication so we push the int number onto the stack and call a
        ;; common handler.
        ;;
        ;; Also, some interrupts push an error code. So we wouldn't always
        ;; have a similar stack frame, so we couldn't call a common
        ;; function. Thus, we need to push a dummy error code if the
        ;; interrupt wont.
        
%macro ISR_NOERR 1
    [GLOBAL isr%1]
    isr%1:
        cli
        push byte 0
        push byte %1
        jmp isr_common_stub
%endmacro

%macro ISR_ERR 1
    [GLOBAL isr%1]
    isr%1:
        cli
        push byte %1
        jmp isr_common_stub
%endmacro

        
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

[EXTERN isr_handler]            ; Defined in C
        ;; The common ISR stub
        ;; saves CPU state (pointers, segment registers, flags),
        ;; sets up kernel-mode segments, calls C-level handler,
        ;; restores CPU state

isr_common_stub:
        pusha                   ; Push edi, esi, ebp, esp, ebx, ecx, eax

        mov ax, ds              ; Push DS
        push eax

        mov ax, 0x10            ; Load kernel data segment descriptor
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        call isr_handler

        pop ebx                 ; Reload DS descriptor
        mov ds, bx
        mov es, bx
        mov fs, bx
        mov gs, bx

        popa                    ; Pop edi et al.
        add esp, 8              ; Cleans up the pushed error code and ISR num
        sti
        iret                    ; Pops CS, EIP, EFLAGS, SS and ESP
