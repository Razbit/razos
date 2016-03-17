;; syscall.s -- Syscall Assembly helpers

;; Copyright (c) 2015, 2016 Eetu "Razbit" Pesonen
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

[EXTERN syscall_dispatch]       ; syscall.c
[GLOBAL syscall_init]           ; syscall.h

%define IA32_SYSENTER_CS  0x0174
%define IA32_SYSENTER_ESP 0x0175
%define IA32_SYSENTER_EIP 0x0176

%define KERNEL_CODE 0x08        ; see gdt.h
%define KENREL_DATA 0x10

syscall_init:
    xor edx, edx

    ;; set up the MSR for sysenter/sysexit
    mov ecx, IA32_SYSENTER_CS
    mov eax, KERNEL_CODE
    wrmsr

    mov ecx, IA32_SYSENTER_ESP
    mov eax, 0x0FFFFFF0         ; sysenter stack is syscall stack
    wrmsr

    mov ecx, IA32_SYSENTER_EIP
    mov eax, syscall_entry      ; We jump there upon syscall
    wrmsr

    ret

syscall_entry:
    pusha

    ;; create fake stack frame
    push dword 0
    push dword 0
    mov ebp, esp

    lea eax, [esp + 8]          ; last element of pusha at [esp+8]
    push eax

    call syscall_dispatch       ; syscall.c

    add esp, 12                 ; clean up the stack

    popa

    sti
    sysexit                     ; go to PL=3, perform_syscall.ret in crt0.s
