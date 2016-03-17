;; gdt.s -- Global Descriptor Table assembly part

;; Copyright (c) 2015 Eetu "Razbit" Pesonen
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
