/* power.c -- power handling */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
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
