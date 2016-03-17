/* console.h -- kernel i/o system */

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

#ifndef CONSOLE_H
#define CONSOLE_H

/* Clear the screen */
void clear_scr();

/* Set color info */
int set_color(int bg, int fg);

/* Print using bg, fg as colors */
int kputchar_color(char c, int bg, int fg);

/* Print using color information set by set_color */
int kputchar(char c);
int kputs(char *str);

int kprintf(const char* fmt, ...);

#endif /* CONSOLE_H */
