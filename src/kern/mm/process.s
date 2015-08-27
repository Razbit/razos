    ;; This file is a part of the Razos project
    ;;
    ;; process.s -- multitasking/process handling assembly
    ;;
    ;; Razbit 2015

    ;;  Called from task.c
[GLOBAL read_eip]
read_eip:
    pop eax                     ; return value (EIP) is on the stack
    jmp eax                     ; JMP to the EIP. Now, the eip is the
                                ; return value (__cdecl)
    ;;  Called from task.c
[GLOBAL do_switch]
do_switch:    
    pop ecx                     ; Load the EIP
    pop esp                     ; Load the ESP
    pop ebp                     ; Load the EBP
    pop edx                     ; Load the new page directory
    mov cr3, edx
    mov eax, 0xB16B00B5         ; The magic boobies
    sti
    jmp ecx                     ; Jump to the loaded EIP
    
