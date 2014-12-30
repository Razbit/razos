/* This file is a part of the RazOS project
 *
 * portio.h -- defines port i/o functions
 *
 * Razbit 2014 */

#ifndef PORTIO_H
#define PORTIO_H

#include <sys/types.h>

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

#endif /* PORTIO_H */
