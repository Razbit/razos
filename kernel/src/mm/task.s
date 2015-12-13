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

    ;; see gdt.h
%define USER_CODE (0x18 | 3)
%define USER_DATA (0x20 | 3)

%define task_fpu_state(task)     [(task) + 0]
%define task_esp(task)           [(task) + 0x200]
%define task_eip(task)           [(task) + 0x204]
%define task_kstack(task)        [(task) + 0x208]
%define task_page_dir_phys(task) [(task) + 0x210]

sched_begin:
    mov ax, USER_DATA
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
	
    mov ecx, 0xFFC00000         ; user stack end: paging.h
    mov edx, 0x10000000         ; task entry point
	
    sti                         ; Now the PIT may call sched_switch
    ;sysexit 	                ; Enter the user world

sched_switch:
    ;; save old task state
    pusha
    mov eax, [cur_task]
    fxsave task_fpu_state(eax)
    mov task_esp(eax), esp
    mov task_eip(eax), dword .return

    call sched_next             ; fetch next task to be run
    mov [cur_task], eax

    mov ebx, task_page_dir_phys(eax) ; restore page directory
    mov cr3, ebx

    fxrstor task_fpu_state(eax)
    mov esp, task_esp(eax)
    jmp task_eip(eax)
.return:
    popa
    ret

task_fork:
    xor eax, eax

    pusha
    call task_fork_inner         ; create & set up the new task

    ;; We save the return value so the parent that calls us gets
    ;; the right task_t* for the new child
    mov [esp + 7*4], eax        ; EAX from pusha

    mov task_esp(eax), esp
    mov task_eip(eax), dword .return
.return:
    popa
    ret
    
