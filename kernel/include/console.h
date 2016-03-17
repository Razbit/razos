/* This file is a part of the RazOS project
 *
 * console.h -- kernel i/o system
 *
 * Razbit 2014, 2016 */

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
