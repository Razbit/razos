/* Define kernel's C-side entry point
 * 
 * Razbit 2014 
 */

#include "kio.h"
#include "gdt.h"
#include "idt.h"
#include "irq.h"

int kmain(struct multiboot *mboot_ptr)
{
	/* Init routines here.. */
	kclear_scr();

    init_gdt();
    init_idt();
    init_irq();
    
    asm volatile("int $0x0");
        
	kprintf("\n==HALTED==");
	return 0xDEADBABE;
}

