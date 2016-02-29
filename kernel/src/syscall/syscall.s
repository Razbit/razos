    ;; This file is a part of the RazOS project
    ;;
    ;; syscall.s -- Syscall Assembly helpers
    ;;
    ;; Razbit 2015 (based on Charlie Somerville's Radium)

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
    sysexit 					; go to PL=3, perform_syscall.ret in crt0.s
