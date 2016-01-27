    ;; This file is a part of the RazOS project
    ;;
    ;; crt0.s -- User side of syscall interface, program startup
    ;;
    ;; Razbit 2015

[GLOBAL _start]

[GLOBAL __syscall0]
[GLOBAL __syscall1]
[GLOBAL __syscall2]
[GLOBAL __syscall3]
[GLOBAL exit]

[EXTERN main]
[EXTERN do_exit]
[EXTERN init_rlibc]
[EXTERN _init]
[EXTERN _fini]

SECTION .start

    ;; Entry of the user program.
_start:
    ;; The execve() -syscall stores contents of argv, envp on the stack:
    ;; <envp contents><envp><argv contents><argv><argc>
    ;; pointer to envp is in esi, pointer to argv in edi

    pop eax                     ; argc in eax

    push esi                    ; push envp
    push edi                    ; push argv
    push eax                    ; push argc

    ;; Prepare signals, memory allocation, stdio, errno etc.
    ;; In setup_rlibc.c
    call init_rlibc

    ;; Global constructors (crti.s)
    call _init
    
    call main
    push eax

    call exit
    
SECTION .text
exit:
    mov ebx, [esp + 4]

    call _fini
    push ebx

    call do_exit

    
    ;; Syscall interface
    
%macro perform_syscall 0
    push ecx
    push edx
    mov ecx, esp                ; sysenter needs ecx, edx
    mov edx, .ret
    sysenter
.ret:
    pop edx
    pop ecx
%endmacro
    
    ;; No parameters, syscall num on stack
    ;; In razos.h as uint32_t __syscall0(num)
__syscall0:
    mov eax, [esp+0+4]

    perform_syscall

    ret

    ;; One parameter and syscall number on stack
    ;; In razos.h as uint32_t __syscall1(num, arg1)
__syscall1:
    push ebx
    mov ebx, [esp+4+8]
    mov eax, [esp+4+4]
    
    perform_syscall

    pop ebx
    ret

    ;; Two parameters and syscall number on stack
    ;; In razos.h as uint32_t __syscall2(num, arg1, arg2)
__syscall2:
    push ebx
    push edi
    mov edi, [esp+8+12]
    mov ebx, [esp+8+8]
    mov eax, [esp+8+4]

    perform_syscall

    pop edi
    pop ebx
    ret

    ;; Three parameters and syscall number on stack
    ;; In razos.h as uint32_t __syscall3(num, arg1, arg2, arg3)
__syscall3:
    push ebx
    push edi
    push esi
    mov esi, [esp+12+16]
    mov edi, [esp+12+12]
    mov ebx, [esp+12+8]
    mov eax, [esp+12+4]

    perform_syscall

    pop esi
    pop edi
    pop ebx
    ret
