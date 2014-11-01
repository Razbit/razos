/* Define kernel's C-side entry point
 * 
 * Razbit 2014 
 */

#include "kio.h"
#include "descriptor_tables.h"

int kmain(struct multiboot *mboot_ptr)
{
	/* Init routines here.. */
	kclear_scr();
    init_descr_tables(); /* GDT, IDT */
    
    asm volatile("int $0x0");
        
	kprintf("\n==HALTED==");
	return 0xDEADBABE;
}

