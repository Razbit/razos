/* This file is a part of the RazOS project
 *
 * kio.c -- kernel i/o system
 *
 * Razbit 2014 */

#include "kio.h"
#include "vga.h"

#include <vsprintf.h>
#include <stdint.h>
#include <stdarg.h>
#include "colors.h"

/* write a character to the screen */
int kputchar(char c)
{
	return vga_putchar(c, COLOR_BLACK, COLOR_WHITE);
}

void kclear_scr()
{
	vga_clear_scr();
}

/* print a null-terminated string to the screen */
int kputs(char *c)
{
	int i = 0;
	while (c[i])
		kputchar(c[i++]);

	return 1;
}

int kprintf(const char* fmt, ...)
{
    char str[1024];

    va_list args;
    va_start(args, fmt);

    int ret = vsprintf(str, fmt, args);

    va_end(args);
    
    kputs(str);

    return ret;
}
