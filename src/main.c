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
	char str[40];
	char str2[] = " & Paste";
	
	strcpy(str, "Copy");	
	strncat(str, str2, 5);

	monitor_write(str);
	
	
	return 0xDEADBABA;
}

