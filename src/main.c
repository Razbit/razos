// main.c Defines the C-code kernel entry point and calls initialization routines.
// JamesM's tuts

#include "lib/common.h"
#include "lib/string.h"
#include "lib/stddef.h"
#include "lib/stdint.h"
#include "lib/ctype.h"

#include "monitor.h"

int main(struct multiboot *mboot_ptr)
{
	// All init calls here..
	monitor_clear();
	
	char str[] ="This, a sample string.";
int i = 0;
  	while (str[i])
	{
		str[i] = tolower(str[i]);
		i++;
	}
monitor_write(str);
	monitor_write("halted");
	return 0xDEADBABA;
}

