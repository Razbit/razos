/* Define kernel's C-side entry point
 * 
 * Razbit 2014 
 */

#include "../lib/common.h"
#include "../lib/string.h"
#include "../lib/stddef.h"
#include "../lib/stdint.h"
#include "../lib/ctype.h"

#include "kio.h"

int kmain(struct multiboot *mboot_ptr)
{
	// All init calls here..
	clear_scr();
	
	char str[] ="This, a sample string.";
int i = 0;
  	while (str[i])
	{
		str[i] = tolower(str[i]);
		i++;
	}
	kputs(str);

	kputs("\n==HALTED==");
	return 0xDEADBABA;
}

