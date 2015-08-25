	;; This file is a part of the RazOS project
	;;
	;; interrupt.s -- ASM side of interrupt handling
	;;
	;; Razbit 2014

	;; We dont receive any info on the interrupt when it fires, so we
	;; need a different handler for each of them. We want to keep clear of
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
	    cli                         ; Disable interrupts
	    push byte 0                 ; Push dummy error code
	    push byte %1                ; Push interrupt number
	    jmp isr_common_stub         ; Jump to the common handler
%endmacro

	%macro ISR_ERR 1
	    [GLOBAL isr%1]
	    isr%1:
	        cli                     ; Disable interrupts
	        push byte %1            ; Push interrupt number
	        jmp isr_common_stub     ; Jump to the common handler
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


[EXTERN isr_handler]            ; in isr.c
;; Saves the processor state (see register_t in isr.h)
;; Sets up kernel-mode segments
;; Calls the C handler and restores the stack frame

isr_common_stub:
    pusha                       ; Push edi, esi, ebp, esp, ebx, ecx, eax

	mov ax, ds                  ; Push DS
	push eax
    
	mov ax, 0x10                ; Load kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call isr_handler

	pop ebx                     ; Reload DS descriptor
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx

	popa                        ; Pop edi et al.
	add esp, 8                  ; Cleans up the pushed error code and ISR num
	iret                        ; Pops CS, EIP, EFLAGS, SS and ESP



    ;; This is for IRQs (Interrupt Requests).
	;; The first param is the IRQ number, the second is the
	;; ISR number mapped to it.
%macro IRQ 2
	[GLOBAL irq%1]
    irq%1:
	    cli                     ; Disable interrupts
	    push byte 0             ; Push dummy error code
	    push byte %2            ; Push interrupt number
	    jmp irq_common_stub
%endmacro
	        
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

	        
[EXTERN irq_handler]            ; In irq.c
	;; Saves the processor state (see register_t in isr.h)
	;; Sets up kernel-mode segments
	;; Calls the C handler and restores the stack frame

irq_common_stub:
	pusha                       ; Push edi, esi, ebp, esp, ebx, ecx, eax
    
	mov ax, ds                  ; Push DS
	push eax
    
	mov ax, 0x10                ; Load kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
    
	call irq_handler
    
	pop ebx                     ; Reload DS descriptor
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx

	popa                        ; Pop edi et al.
	add esp, 8                  ; Cleans up the pushed error code and ISR num
	iret                        ; Pops CS, EIP, EFLAGS, SS and ESP
