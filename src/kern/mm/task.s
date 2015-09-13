    ;; This file is a part of the RazOS project
    ;;
    ;; task.s -- tasking functionality
    ;;
    ;; Razbit 2015

[EXTERN fork_inner]             ; In task.c
[GLOBAL do_fork]                ; In task.h: struct task_t* fork_inner()
    
do_fork:
    xor eax, eax                ; Set EAX to zero (return value to child)

    pusha                       ; eax, ecx, edx, ebx, esp, ebp, esi, edi
    call fork_inner             ; In task.c

    ;; Save return value of fork_inner for returning to the parent
    mov [esp + 7*4], eax        ; [esp + 7*4] is EAX by pusha above

    ;TODO: figure out the following {references}
    mov {task_esp_from_task_in_eax}, esp 
    mov {task_eip_from_task_in_eax}, dword .ret

.ret:
    popa
    ret
