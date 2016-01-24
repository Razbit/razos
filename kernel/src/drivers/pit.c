/* This file is a part of the RazOS project
 *
 * pit.c -- Programmable Interrupt Timer
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <portio.h>

#include "pit.h"

#define PIT_FREQ 1193180

void pit_set_freq(uint32_t freq)
{
	uint32_t divisor = PIT_FREQ / freq;

	outb(0x43, 0x36);
	outb(0x40, divisor & 0xff);
	outb(0x40, (divisor >> 8) & 0xff);
}
