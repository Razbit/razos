;; boot.s -- Kernel start location. Also defines multiboot header.

;; Copyright (c) 2014, 2015 Eetu "Razbit" Pesonen
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
