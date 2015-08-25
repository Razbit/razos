    ;; This file is a part of the RazOS project
    ;;
    ;; idt.s -- writes the IDT pointer
    ;;
    ;; Razbit 2014

[GLOBAL idt_flush]              ; Allow calling from C
    
idt_flush:                      ; Write the IDT pointer passed from C
    mov eax, [esp+4]            ; First parameter is the pointer to the IDT
    lidt [eax]                  ; Load the pointer
    ret
