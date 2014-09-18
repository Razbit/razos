// main.c Defines the C-code kernel entry point and calls initialization routines.
// JamesM's tuts

#include "lib/common.h"
#include "monitor.h"

int main(struct multiboot *mboot_ptr)
{
	// All init calls here..
	monitor_clear();
	monitor_write("Hello, World!\n");
	monitor_write_dec(128);
	monitor_write("\n");
	monitor_write_dec(1);
	monitor_write("\n");
	monitor_write_dec(80);
	monitor_write("\n");
	monitor_write_dec(0);
	monitor_write("\n");
	
	return 0xDEADBABA;
}

