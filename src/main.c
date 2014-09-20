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
	char str[] = "abc";
	char str2[] = "abcd";
	
	int comp = strncmp(str, str2, 3);
	monitor_write_dec(comp);
	
	
	return 0xDEADBABA;
}

