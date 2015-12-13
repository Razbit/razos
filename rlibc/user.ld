ENTRY(_start)
SECTIONS {
    . = 0x10000000;

    .text ALIGN (0x1000) : {
	*(.start)
        *(.crt0)
        *(.init)
        *(.text)
        *(.fini)
    }

    .rodata ALIGN (0x1000) : {
        *(.rodata)
    }

    .data ALIGN (0x1000) : {
        *(.data)
    }

    .bss ALIGN (0x1000) : {
        sbss = .;
        *(COMMON)
        *(.bss)
        ebss = .;
    }
}
