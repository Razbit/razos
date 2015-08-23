;
; boot.s -- Kernel start location. Also defines multiboot header.
; Based on Bran's kernel development tutorial file start.asm
;

MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


[BITS 32]                       ; All instructions should be 32-bit.

[GLOBAL mboot]                  ; Make 'mboot' accessible from C.
[EXTERN code]                   ; Start of the '.text' section.
[EXTERN bss]                    ; Start of the .bss section.
[EXTERN end]                    ; End of the last loadable section.

mboot:
    dd MBOOT_HEADER_MAGIC       ; GRUB will search for this value on each
                                ; 4-byte boundary in your kernel file
    dd MBOOT_HEADER_FLAGS       ; How GRUB should load your file / settings

    dd MBOOT_CHECKSUM           ; To ensure that the above values are correct

[GLOBAL start]                  ; Kernel entry point.
[EXTERN kmain]                  ; This is the entry point of our C code

start:
    mov esp, stack_end          ; Set the stack
    push esp                    ; Push stack location
    push ebx                    ; Load multiboot header location

    cli                         ; Disable interrupts.
    call kmain                  ; C entry


    cli                         ; Go to infinite loop if kmain() returns
hang:
    hlt
    jmp hang
        
stack_start:
    resb 0xFFFF                 ; 64 KB of stack space
stack_end:
