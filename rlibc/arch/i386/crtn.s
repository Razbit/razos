    ;; This file is a part of the RazOS project
    ;;
    ;; crtn.s -- provides the kernel with crtn.o
    ;;
    ;; Razbit 2015

SECTION .init
_init:
    ;; GCC will insert his crap here
    pop ebp
    ret

SECTION .fini
_fini:
    ;; GCC will insert his crap here
    pop ebp
    ret
