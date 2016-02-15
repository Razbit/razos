    ;; This file is a part of the RazOS project
    ;;
    ;; task.s -- Assembly helpers for tasking
    ;;
    ;; Razbit 2015 (based on Charlie Somerville's Radium)

[GLOBAL sched_begin]            ; sched.h
[GLOBAL sched_switch]           ; sched.h
[GLOBAL task_fork]              ; task.h

[EXTERN cur_task]               ; task.c
[EXTERN sched_next]             ; task.c
[EXTERN task_fork_inner]        ; task.c
[EXTERN set_page_dir]           ; paging.c
[EXTERN kprintf]
[EXTERN page_flags]

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

;	push esi
;	push edi
;	push ecx
;	push .msg
;	call kprintf

;	sub esp, 16

    mov eax, USER_DATA
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

    mov edx, 0x10000000         ; task entry point
    sti                         ; Now the PIT may call sched_switch
    sysexit                     ; Enter the user world

;	.msg db "stack: 0x%p argv: 0x%p envp: 0x%p", 0

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

    ;; restore page directory
    push dword task_page_dir(eax)
    call set_page_dir
	add esp, 4
;	push dword task_page_dir(eax)
;	push 0x0FFFFFF0
;	call page_flags
;	push eax
;	push .msg
;	call kprintf
;	add esp, 16
;	jmp $
	;; restore eax
	mov eax, [cur_task]

    ;; restore task state
    fxrstor task_fpu_state(eax)
    mov esp, task_esp(eax)
    jmp task_eip(eax)
.return:
    popa
    ret

.msg db "sched: 0x0ffffff0 flags: 0x%p", 0x0a,  0

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
