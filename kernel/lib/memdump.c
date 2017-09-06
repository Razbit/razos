/* memdump.c -- UNIX hexdump -like utility */

/* Copyright (c) 2014, 2017 Eetu "Razbit" Pesonen
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

/* This version only prints the kind of output you get when
 * passing -C to the UNIX hexdump */

#include <memdump.h>
#include <stdint.h>
#include <console.h>
#include <ctype.h>

void memdump(void* addr, size_t num)
{
	uint8_t* buf = (uint8_t*)addr;

	if (num == 0)
		return;

	size_t n, i, j = 0;

	do
	{
		kprintf("%8.8x  ", j);

		if (num <= 16)
		{
			n = num;
			num = 0;
		}
		else
		{
			n = 16;
			num -= 16;
		}

		for (i = 0; i < n; i++)
		{
			if (i > 0 && i % 8 == 0)
				kprintf(" ");

			kprintf("%2.2x ", *(buf+i));
		}

		if (n > 0)
		{
			if (i < 8)
				kprintf(" ");

			for (; i < 16; i++)
				kprintf("   ");
			kprintf(" |");
		}

		for (i = 0; i < n; i++)
		{
			if (isprint(*(buf+i)))
				kprintf("%c", *(buf+i));
			else
				kprintf(".");
		}

		if (n > 0)
			kprintf("|\n");

		j += n;
		buf += n;
	} while (n > 0);

	kprintf("\n");

	return;
}
