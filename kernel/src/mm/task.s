;; task.s -- Assembly helpers for tasking

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

[GLOBAL sched_begin]            ; sched.h
[GLOBAL sched_switch]           ; sched.h
[GLOBAL task_fork]              ; task.h

[EXTERN cur_task]               ; task.c
[EXTERN sched_next]             ; task.c
[EXTERN task_fork_inner]        ; task.c
[EXTERN sched_cont]             ; task.c
[EXTERN get_page_dir_phys]      ; paging.c

    ;; see gdt.h
%define USER_CODE (0x18 | 3)
%define USER_DATA (0x20 | 3)

    ;; see task.h
%define task_fpu_state(task) [(task) + 0]
%define task_esp(task)       [(task) + 0x200]
%define task_eip(task)       [(task) + 0x204]
%define task_page_dir(task)  [(task) + 0x208]

sched_begin:
    mov esi, [esp + 12]         ; envp
    mov edi, [esp + 8]          ; argv
    mov ecx, [esp + 4]          ; user stack end

    call sched_cont

    mov eax, USER_DATA
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

    mov edx, 0x10000000         ; task entry point
    sti                         ; Now the PIT may call sched_switch
    sysexit                     ; Enter the user world

sched_switch:
    ;; save old task state
    pusha
    mov eax, [cur_task]
    fxsave task_fpu_state(eax)
    mov task_esp(eax), esp
    mov task_eip(eax), dword .return

    ;; fetch the next task
    call sched_next
    mov [cur_task], eax

    ;; get physical page dir address
    push dword task_page_dir(eax)
    call get_page_dir_phys
    add esp, 4
    ;; eax now contains physical address of the new task's page dir
    ;; load to cr3
    mov cr3, eax

    ;; restore eax
    mov eax, [cur_task]

    ;; restore task state
    fxrstor task_fpu_state(eax)
    mov esp, task_esp(eax)
    jmp task_eip(eax)

.return:
    popa
    ret

task_fork:
    xor eax, eax

    pusha
    call task_fork_inner        ; create & set up the new task

    ;; We save the return value so the parent that calls us gets
    ;; the right task_t* for the new child
    mov [esp + 7*4], eax        ; EAX from pusha

    mov task_esp(eax), esp
    mov task_eip(eax), dword .return

.return:
    popa
    ret
