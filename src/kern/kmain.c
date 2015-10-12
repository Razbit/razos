/* This file is a part of the RazOS project
 *
 * Define kernel's C-side entry point
 * 
 * Razbit 2014, 2015 
 */

#include <console.h>
#include "gdt.h"
#include "interrupt/idt.h"
#include "interrupt/pit.h"
#include "mm/paging.h"
#include "mm/kernel_page.h"


#include <kassert.h>
#include <sys/types.h>
#include <asm/system.h>
#include <asm/multiboot.h>

extern uint32_t placement_addr;

int kmain(struct multiboot_info* mb, uint32_t esp)
{
    /* Init routines here.. */
    kclear_scr();

    gdt_init();
    idt_init();

    kprintf("Stack is at 0x%p\n", esp);


    pit_set_freq(100);

    paging_init(mb);
    
    kprintf("\n==HALTED==");

    for(;;);
    
	return 0xDEADBABE;
}

