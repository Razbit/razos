/* This file is a part of the RazOS project
 *
 * console.c -- kernel i/o system
 *
 * Razbit 2014 */

#include <console.h>
#include "vga.h"

#include <vsprintf.h>
#include <sys/types.h>
#include <stdarg.h>
#include <string.h>
#include <kmalloc.h>

#include "colors.h"

struct line_t
{
	uint16_t str[80];
	struct line_t* next;
	struct line_t* prev;
};

static struct line_t* line_buf = NULL;
static struct line_t* bot_line = NULL;  /* Last visible line */
static struct line_t* last_line = NULL; /* Last line of the buffer */

static int status = 0;
static int x_pos = 0;

/* Color information */
static uint8_t cur_bg = COLOR_BLACK;
static uint8_t cur_fg = COLOR_WHITE;

/* A buffer for use by kprintf */
static char* kprintf_buf = NULL;

void console_init()
{
	line_buf = (struct line_t*)kmalloc(sizeof(struct line_t));
	last_line = bot_line = line_buf;

	last_line->str = {0};
	last_line->next = NULL;
	last_line->prev = NULL;

	/* Make sure we start at a blank line */
	kputchar('\n');
	
	status = CONSOLE_INIT;
}

/* Refresh the console */
void console_refresh()
{
	if (status != CONSOLE_INIT)
		return;

	struct line_t* line = bot_line;

	for (int i = 0; i < 24; i++)
	{
		if (line->prev == NULL)
			break;
		line = line->prev;
	}

	for (int i = 0; i < 25; i++)
	{
		if (line == NULL)
			break;

		for (int j = 0; j < 80; j++)
			vga_putchar(line->str[j]);

		line = line->next;
	}
}

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
	/* Print directly to the screen if the line buffer system is not
	 * yet available */
	if (status == CONSOLE_INIT)
	{
		if (x_pos >= 80)
		{
			/* Create a new line */
			x_pos = 0;
			last_line->next = \
				(struct line_t*)kmalloc(sizeof(struct line_t));
			last_line->next->prev = last_line;
			last_line->next->next = NULL;
			last_line = last_line->next;
			last_line->str = {0};
		}

		last_line->str[x_pos++] = vga_mkchar(c, bg, fg);
		bot_line = last_line;
		
		if (c == '\n')
			console_refresh();
	}
	else
	{
		return vga_putchar(vga_mkchar(c, bg, fg));
	}
}

/* write a character to the screen using default colors */
int kputchar(char c)
{
	return kputchar_color(c, cur_bg, cur_fg);
}

/* Scroll the terminal up or down */
void scroll(int lines)
{
	if (status != CONSOLE_INIT)
		return;
	if (lines == 0)
		return;
	
	if (lines < 0) /* Scroll down */
	{
		//
	}
	else /* Scroll up */
	{
		//
	}
	
}

void clear_scr()
{
	if (status == CONSOLE_INIT)
	{
		for (int i = 0; i < 25*80; i++)
			kputchar('\0');
	}
	else
	{
		vga_clear_scr();
	}
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
	if (kprintf_buf = NULL)
		kprintf_buf = (char*)kmalloc(sizeof(char) * 4096);

	memset(kprintf_buf, 0, 4096);
	
	va_list args;
	va_start(args, fmt);

	int ret = vsprintf(kprintf_buf, fmt, args);

	va_end(args);

	kputs(kprintf_buf);

	return ret;
}
