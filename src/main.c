// main.c Defines the C-code kernel entry point and calls initialization routines.
// JamesM's tuts

#include "lib/common.h"
#include "lib/string.h"
#include "lib/stddef.h"
#include "lib/stdint.h"

#include "monitor.h"

int main(struct multiboot *mboot_ptr)
{
	// All init calls here..
	monitor_clear();
	
	char str[] ="- This, a sample string.";
  	char * pch;
  	pch = strtok (str," ,.-");
  	while (pch != NULL)
  	{
    		monitor_write(pch);
		monitor_write("\n");
    		pch = strtok (NULL, " ,.-");
  	}
	monitor_write("halted");
	return 0xDEADBABA;
}

