        ;; This file is a part of the RazOS project
        ;;
        ;; gdt.s -- writes the GDT pointer
        ;;
        ;; Razbit 2014

[GLOBAL gdt_flush]              ; Allow C to call this

gdt_flush:                      ; Write the GDT pointer passed from C
        mov eax, [esp+4]        ; The first param
        lgdt [eax]              ; Load GDT pointer

        mov ax, 0x10            ; 0x10: the offset in the GDT to kern data seg
        mov ds, ax              ; Load the data segment selectors
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        jmp 0x08:.flush         ; 0x08 is the offset to our code seg
.flush:                         ; The far jump effectively changes the CS
        ret