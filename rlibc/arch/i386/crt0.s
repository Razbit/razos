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

[EXTERN main]
[EXTERN exit]
[EXTERN _init]
[EXTERN _fini]

SECTION .text

    ;; Entry of the user program.
    ;; Loader puts *argv, *envp, argc, argv, envc, envp on stack
_start:
    ;; Prepare signals, memory allocation, stdio etc.
    ;; Uncomment when done.
    ;; call init_rlibc

    ;; Global constructors (crti.s)
    call _init
    
    call main
    
    push eax

    call _exit
    
_exit:
    pop ebx
    call _fini
    push ebx
    call exit

    
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
    
    ;; No parameters, syscall num on stored on stack
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
