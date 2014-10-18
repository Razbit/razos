/* Define kernel's C-side entry point
 * 
 * Razbit 2014 
 */

#include "kio.h"

int kmain(struct multiboot *mboot_ptr)
{
	// All init calls here..
	kclear_scr();
    	
	kprintf("\n==HALTED==");
	return 0xDEADBABA;
}

