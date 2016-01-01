/* This file is a part of the RazOS project
 *
 * pagefault.c -- pagefault handling
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <panic.h>
#include <console.h>

#include "task.h"
#include "paging.h"

/* Handle pagefaults. Called in ISR 14 in isr.s */
/* TODO: highly WIP */
void pagefault_handler(uint32_t addr, uint32_t error, uint32_t eip)
{
	char err[5] = {0};
	if (error & 0x4)
		err[0] = 'U';
	else
		err[0] = 'S';
	err[1] = '-';
	if (error & 0x2)
		err[2] = 'W';
	else
		err[2] = 'R';
	err[3] = '-';
	if (error & 0x1)
		err[4] = 'P';
	else
		err[4] = 'N';
	err[5] = '\0';
	
	panic("Page fault at 0x%p (0x%x: %s) eip: 0x%p\n", \
	      addr, error, err, eip);
}
