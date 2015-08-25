    ;; This file is a part of the RazOS project
	;;
	;; boot.s -- Kernel start location. Also defines multiboot header.
	;; Based on Bran's kernel development tutorial file start.asm
	;;
	;; Razbit 2014-2015

SECTION .bss
	        
stack_start:
	resb 0xFFFF                 ; 64 KB of stack space
stack_end:

fpu_testword:
	resw 0x0001
    
	        
SECTION .text
	        
MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


[BITS 32]                       ; All instructions should be 32-bit.

mboot:
	dd MBOOT_HEADER_MAGIC       ; GRUB will search for this value on each
	dd MBOOT_HEADER_FLAGS       ; How GRUB should load your file / settings
	dd MBOOT_CHECKSUM           ; To ensure that the above values are correct

	        
[GLOBAL start]                  ; Kernel entry point.
[EXTERN kmain]                  ; This is the entry point of our C code

start:
	mov edx, cr0                ; Test if we have floating-point support
	and edx, (-1) - (0x4 + 0x8)
	mov cr0, edx
	mov [fpu_testword+4], word 0x55aa      
    
	fninit                      ; Init the FPU
	
	fnstsw [fpu_testword+4]
	cmp word [fpu_testword+4], 0
	jne hang                    ; Hang if don't have floats (for now)
	
	mov esp, stack_end          ; Set the stack
	push esp                    ; Push stack location
	push ebx                    ; Load multiboot header location

	cli                         ; Disable interrupts.
	call kmain                  ; C entry


	cli                         ; Go to infinite loop if kmain() returns

hang:
	hlt
	jmp hang
	        
