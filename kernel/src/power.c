/* This file is a part of the RazOS project
 *
 * power.h -- power handling
 *
 * Razbit 2016 */

#include <asm/power.h>
#include <portio.h>
#include <console.h>
#include <panic.h>

void shutdown()
{
	kprintf("Shutting down...\n");
	/* extremely proper ACPI usage.. :D */
	outw(0xb004, 0x0 | 0x2000);
	panic("power: lol no, just panicing xD\n");
}
