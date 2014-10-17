/* Define kernel's C-side entry point
 * 
 * Razbit 2014 
 */

#include "../lib/common.h"
#include "../lib/string.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/ctype.h"
#include "../lib/vsprintf.h"
#include "../lib/stdarg.h"

#include "kio.h"

int kmain(struct multiboot *mboot_ptr)
{
	// All init calls here..
	clear_scr();
    
    kputs("\n==HALTED==");
	return 0xDEADBABA;
}

