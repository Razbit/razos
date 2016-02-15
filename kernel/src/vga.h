/* This file is a part of the RazOS project
 *
 * vga.h -- the VGA terminal driver system
 *
 * Razbit 2014, 2016 */

#ifndef VGA_H
#define VGA_H

#include <sys/types.h>

void vga_clear_scr();
uint16_t vga_mkchar(char c, uint8_t bg_color, uint8_t fg_color);
int vga_putchar(uint16_t c);

#endif /* VGA_H */
