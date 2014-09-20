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
	char str[] = "Hello, World!\n";
	monitor_write(str);
	char str2[] = "Oh hi there World!\n";
	memcpy(str2, str, 8);
	monitor_write(str2);
	
	
	return 0xDEADBABA;
}

