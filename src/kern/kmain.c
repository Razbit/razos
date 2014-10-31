/* Define kernel's C-side entry point
 * 
 * Razbit 2014 
 */

#include "kio.h"

int kmain(struct multiboot *mboot_ptr)
{
	/* Init routines here.. */
	kclear_scr();

    asm volatile("int $0x0");
    
	kprintf("\n==HALTED==");
	return 0xDEADBABA;
}

