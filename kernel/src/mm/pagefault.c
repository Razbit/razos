/* pagefault.c -- pagefault handling */

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
#include <panic.h>
#include <console.h>

#include "task.h"
#include "paging.h"

/* TODO: We should check for 'valid' (i.e. out-of-memory)
 * faults and fix them */

/* Handle pagefaults. Called in ISR 14 in isr.s */
void pagefault_handler(uint32_t addr, uint32_t error, uint32_t eip)
{
	char err[6] = {0};
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

	kprintf("Page fault at %p (0x%x: %s) eip: %p\n", \
	      addr, error, err, eip);

	panic("Page fault\n");
}
