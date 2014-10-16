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

void sprintf(char* str, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);
}
	
int kmain(struct multiboot *mboot_ptr)
{
	// All init calls here..
	clear_scr();
	
	char str[64];
	memset(str, '\0', 64);

	sprintf(str, "De%irp\n", 3);
	
	kputs(str);

	kputs("\n==HALTED==");
	return 0xDEADBABA;
}

