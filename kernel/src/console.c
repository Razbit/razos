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
#include <kmalloc.h>

#include <colors.h>

#define CONSOLE_INIT 1
#define CONSOLE_AVAILABLE 2

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
static int nlines = 0;

/* Color information */
static uint8_t cur_bg = COLOR_BLACK;
static uint8_t cur_fg = COLOR_WHITE;

/* A buffer for use by kprintf */
static char kprintf_buf[4096] = {0};

static struct line_t* create_line(struct line_t* prev_line)
{
	prev_line->next = (struct line_t*)kmalloc(sizeof(struct line_t));
	prev_line->next->prev = prev_line;
	prev_line->next->next = NULL;
	memset(prev_line->next->str, 0, 80);

	nlines++;
	
	if (nlines > 500)
	{
		line_buf = line_buf->next;
		kfree(line_buf->prev);
		line_buf->prev = NULL;
				
		nlines--;
	}

	return prev_line->next;
}

void console_init()
{
	line_buf = (struct line_t*)kmalloc(sizeof(struct line_t));
	last_line = bot_line = line_buf;

	memset(last_line->str, 0, 80);
	last_line->next = NULL;
	last_line->prev = NULL;
	
	nlines++;

	/* Make sure we start at a blank line */
	kputchar('\n');

	status = CONSOLE_INIT;

	/* Figure out how we could preserve the screen's contents */
	
	status = CONSOLE_AVAILABLE;

	kprintf("Console initialized\n");

	console_refresh();
}

/* Refresh the console */
void console_refresh()
{
	if (status != CONSOLE_AVAILABLE)
		return;

	vga_clear_scr();

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
	if (status == CONSOLE_AVAILABLE || status == CONSOLE_INIT)
	{
		if (x_pos >= 80)
		{
			/* Create a new line */
			x_pos = 0;
			last_line = create_line(last_line);
		}

		last_line->str[x_pos++] = vga_mkchar(c, bg, fg);
		bot_line = last_line;

		if (c == '\n')
		{
			x_pos = 0;
			last_line = create_line(last_line);

			console_refresh();
		}

		return c;
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
	if (status != CONSOLE_AVAILABLE)
		return;
	if (lines == 0)
		return;
	
	if (lines < 0) /* Scroll down */
	{
		for (int i = -lines; i > 0; i--)
		{
			/* We reached the bottom */
			if (bot_line->next == NULL || bot_line == last_line)
				break;
			
			bot_line = bot_line->next;
		}
	}
	else /* Scroll up */
	{
		/* Find the current visible top line */
		struct line_t* top_line = bot_line;

		for (int i = 0; i < 24; i++)
		{
			if (top_line->prev == NULL)
				break;
			top_line = top_line->prev;
		}

		/* Scroll */
		for (int i = lines; i >= 0; i--)
		{
			/* We reached the top */
			if (top_line->prev == NULL)
				break;

			top_line = top_line->prev;
		}

		/* Set the bottom line */
		for (int i = 0; i < 25; i++)
		{
			if (top_line->next == NULL || top_line == last_line)
				break;

			top_line = top_line->next;
		}
		
		bot_line = top_line;
	}

	console_refresh();
}

void clear_scr()
{
	if (status == CONSOLE_AVAILABLE)
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
