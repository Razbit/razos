    ;; This file is a part of the RazOS project
    ;;
    ;; gdt.s -- Global Descriptor Table assembly part
    ;;
    ;; Razbit 2015 (based on Charlie Somerville's Radium)

[EXTERN gdt_ptr]                ; in gdt.c
[GLOBAL gdt_reload]             ; in gdt.h as void gdt_reload()

SECTION .text

gdt_reload:
	cli
    lgdt [gdt_ptr]              ; Load GDT table specified by gdtr

	;; Enable protected mode
	mov eax, cr0
	or al, 1
	mov cr0, eax

	;; Load CS using a far jump
    jmp 0x08:.flush_cs
.flush_cs:
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ss, eax
    ret
