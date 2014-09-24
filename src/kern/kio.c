#include "kio.h"

#include "../lib/common.h"
#include "../lib/math.h"
#include "../lib/string.h"
#include "../lib/stdint.h"

/* The VGA framebuffer starts at 0xB8000 */
uint16_t *video_memory = (uint16_t*)0xB8000;

uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

static void move_cursor()
{
	uint16_t cursor_loc = cursor_y * 80 + cursor_x;
	outb(0x3D4, 14); /* tell the VGA board we are setting the high cursor byte */
	outb(0x3D5, cursor_loc >> 8);
	outb(0x3D4, 15); /* low cursor byte */
	outb(0x3D5, cursor_loc);
}

/* scroll text up one line */
static void scroll()
{
	/* create a 'blank' char.. */
	uint8_t attrib_byte = (COLOR_BLACK << 4) | (COLOR_WHITE & 0x0F);
	uint16_t blank = 0x20 | (attrib_byte << 8); /* 0x20 = ASCII for spc */
	
	if (cursor_y >= 25)
	{
		/* move the text up by one line */
		int i;
		for (i = 0; i < 24*80; i++)
			video_memory[i] = video_memory[i+80];
		
		/* fill last line with emptyness */
		for (i = 24*80; i < 25*80; i++)
			video_memory[i] = blank;

		cursor_y = 24; /* return cursor to the last line */
	}
}


/* write a character to the screen */
int kputchar(char c)
{
	uint8_t bg_color = COLOR_BLACK;
	uint8_t fg_color = COLOR_WHITE;
	
	/* attrib byte contains the bg and fg colors, 4 bits each */
	uint8_t colorattrib = (bg_color << 4) | (fg_color & 0x0F);
	/* attrib byte is the top half of the vga word */
	uint16_t attrib = colorattrib << 8;
	uint16_t *location;
	
	/* backspc handler */
	if (c == 0x08 && cursor_x)
		cursor_x--;
	
	/* tab handler */
	else if (c == 0x09)
		cursor_x = (cursor_x+8) & ~(8-1); /* tab is 8 spaces */
	
	/* carriage ret */
	else if (c == '\r')
		cursor_x = 0;
	
	/* newline handler */
	else if (c == '\n')
	{
		cursor_x = 0;
		cursor_y++;
	}
	
	/* handle any printable chars */
	else if (c >= ' ')
	{
		location = video_memory + (cursor_y*80 + cursor_x);
		*location = (uint16_t)c | attrib;
		cursor_x++;
	}
	
	/* check if we need a newline */
	if (cursor_x >= 80)
	{
		cursor_x = 0;
		cursor_y++;
	}
	
	scroll();
	move_cursor();

	return c;
}

void clear_scr()
{
	/* attrib byte for default colors */
	uint8_t colorattrib = (COLOR_BLACK << 4) | (COLOR_WHITE & 0x0F);
	uint16_t blank = 0x20 | (colorattrib << 8);
	
	int i;
	for (i = 0; i < 80*25; i++)
		video_memory[i] = blank;
	
	/* move cursor back to the start */
	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}

/* print a null-terminated string to the screen */
int kputs(char *c)
{
	int i = 0;
	while (c[i])
		kputchar(c[i++]);

	return 1;
}

