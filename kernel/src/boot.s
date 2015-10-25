    ;; This file is a part of the RazOS project
    ;;
    ;; boot.s -- Kernel start location. Also defines multiboot header.
    ;; Based on Bran's kernel development tutorial file start.asm as well as
    ;; Charlie Somerville's Radium's loader.asm
    ;;
    ;; Razbit 2014-2015

MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide the kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

[BITS 32]                       ; All instructions should be 32-bit.

SECTION .multiboot

mboot:
    dd MBOOT_HEADER_MAGIC       ; GRUB will search for this value on each
    dd MBOOT_HEADER_FLAGS       ; How GRUB should load your file / settings
    dd MBOOT_CHECKSUM           ; To ensure that the above values are correct

SECTION .text

[GLOBAL start]                  ; Kernel entry point
[GLOBAL end_of_image]           ; End of kernel image
[EXTERN kmain]                  ; This is the entry point of our C code

start:
    mov esp, stack_end          ; Setup the stack
    push dword 0
    push dword 0
    mov ebp, esp

    push esp                    ; Push stack location
    push ebx                    ; Load multiboot header location

    fninit                      ; Init the FPU
    mov eax, cr0
    or eax, 1 << 5              ; FPU NE bit
    mov cr0, eax

    cli                         ; Disable interrupts.
    call kmain                  ; C entry

    cli                         ; Go to infinite loop if kmain() returns
    jmp .hang
.hang:
    jmp .hang

SECTION .bss

stack_start:
    resb 0xFFFF                 ; 64 KB of stack space
stack_end:

SECTION .end_of_image
end_of_image:
