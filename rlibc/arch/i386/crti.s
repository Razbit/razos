    ;; This file is a part of the RazOS project
    ;;
    ;; crti.s -- provides the user-space with crti.o
    ;;
    ;; Razbit 2015

SECTION .init
[GLOBAL _init]
_init:
    push ebp                    ; Set up a stack frame
    mov ebp, esp
    ;; GCC will continue from here

SECTION .fini
[GLOBAL _fini]
_fini:
    push ebp
    mov ebp, esp
    ;; GCC will continue from here
    
