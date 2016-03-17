/* vga.h -- the VGA terminal driver system */

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

#ifndef VGA_H
#define VGA_H

#include <sys/types.h>

/* The VGA framebuffer */
extern uint16_t *video_memory;

void vga_clear_scr();
uint16_t vga_mkchar(char c, uint8_t bg_color, uint8_t fg_color);
int vga_putchar(uint16_t c);

#endif /* VGA_H */
