/* portio.c -- port i/o functions */

/* Copyright (c) 2014, 2016 Eetu "Razbit" Pesonen
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

#include <portio.h>
#include <stdint.h>

/* write a byte to the port */
void outb(uint16_t port, uint8_t value)
{
	__asm__ __volatile__("outb %1, %0" :: "dN"(port), "a"(value));
}

/* write a word to the port */
void outw(uint16_t port, uint16_t value)
{
	__asm__ __volatile__("outw %1, %0" :: "dN"(port), "a"(value));
}

/* read a byte from the port */
uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ __volatile__("inb %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

/* read a word from the port */
uint16_t inw(uint16_t port)
{
	uint16_t ret;
	__asm__ __volatile__("inw %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}
