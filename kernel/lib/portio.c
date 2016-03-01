/* This file is a part of the RazOS project
 *
 * portio.c -- port i/o functions
 *
 * Razbit 2014, 2016 */

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
