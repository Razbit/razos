/* pit.c -- Programmable Interrupt Timer */

/* Copyright (c) 2015 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

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
