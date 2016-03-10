/* This file is a part of the RazOS project
 *
 * console.c -- kernel i/o system
 *
 * Razbit 2014, 2016 */

#include <console.h>
#include "vga.h"

#include <vsprintf.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>

#include <colors.h>

/* Color information */
static uint8_t cur_bg = COLOR_BLACK;
static uint8_t cur_fg = COLOR_WHITE;

/* A buffer for use by kprintf */
static char kprintf_buf[4096] = {0};

/* Change color, return previous color info */
int set_color(int bg, int fg)
{
	int ret = (cur_bg) << 4 | (cur_fg & 0x0F);
	
	cur_bg = bg & 0x0F;
	cur_fg = fg & 0x0F;

	return ret;
}

/* write a character to the screen with color info */
int kputchar_color(char c, int bg, int fg)
{
		return vga_putchar(vga_mkchar(c, bg, fg));
}

/* write a character to the screen using default colors */
int kputchar(char c)
{
	return kputchar_color(c, cur_bg, cur_fg);
}

void clear_scr()
{
	vga_clear_scr();
}

/* print a null-terminated string to the screen */
int kputs(char *c)
{
	int i = 0;
	while (c[i])
		kputchar(c[i++]);
	kputchar('\n');
	
	return i;
}

int kprintf(const char* fmt, ...)
{
	memset(kprintf_buf, 0, 4096);
	
	va_list args;
	va_start(args, fmt);

	int ret = vsprintf(kprintf_buf, fmt, args);

	va_end(args);

	int i = 0;
	while (kprintf_buf[i] && i < ret)
		kputchar(kprintf_buf[i++]);

	return ret;
}
