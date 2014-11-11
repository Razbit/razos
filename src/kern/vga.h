/* This file is a part of the RazOS project
 *
 * vga.h -- the VGA terminal driver system
 *
 * Razbit 2014 */

#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_clear_scr();
int vga_putchar(char c, uint8_t bg_color, uint8_t fg_color);

#endif /* VGA_H */
