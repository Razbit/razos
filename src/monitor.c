#include "monitor.h"

#include "lib/common.h"
#include "lib/math.h"
#include "lib/string.h"

// The VGA framebuffer starts at 0xB8000.
uint16_t *video_memory = (uint16_t *)0xB8000;
// Stores the cursor position.
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

//update hw cursor
static void move_cursor()
{
	uint16_t cursor_loc = cursor_y * 80 + cursor_x;
	outb(0x3D4, 14); //tell the VGA board we are setting the high cursor byt
	outb(0x3D5, cursor_loc >> 8); //send high byte
	outb(0x3D4, 15); //low byte
	outb(0x3D5, cursor_loc);
}

//scroll text up one line..
static void scroll()
{
	//create a 'blank' char..
	uint8_t attrib_byte = (0 << 4) | (15 & 0x0F);
	uint16_t blank = 0x20 | (attrib_byte << 8); //0x20 = ASCII for spc
	
	if (cursor_y >= 25)
	{
		//move the text up by one line
		int i;
		for (i = 0; i < 24*80; i++)
		{
			video_memory[i] = video_memory[i+80];
		}
		
		//fill last line with emptyness
		for (i = 24*80; i < 25*80; i++)
		{
			video_memory[i] = blank;
		}
		cursor_y = 24; //return cursor to the last line
	}
}


//write a character to the screen
void monitor_put(char c)
{
	//bg black, fg white
	uint8_t bg_color = 0;
	uint8_t fg_color = 15;
	
	//attrib byte contains the bg and fg colors, 4 bits each
	uint8_t colorattrib = (bg_color << 4) | (fg_color & 0x0F);
	//attrib byte if the top half of the vga word
	uint16_t attrib = colorattrib << 8;
	uint16_t *location;
	
	//backspc handler
	if (c == 0x08 && cursor_x)
	{
		cursor_x--;
	}
	
	//tab handler
	else if (c == 0x09)
	{
		cursor_x = (cursor_x+8) & ~(8-1);
	}
	
	//carriage ret
	else if (c == '\r')
	{
		cursor_x = 0;
	}
	
	//newline handler
	else if (c == '\n')
	{
		cursor_x = 0;
		cursor_y++;
	}
	
	//handle any printable chars
	else if (c >= ' ')
	{
		location = video_memory + (cursor_y*80 + cursor_x);
		*location = (uint16_t)c | attrib;
		cursor_x++;
	}
	
	//check if need a newline
	if (cursor_x >= 80)
	{
		cursor_x = 0;
		cursor_y++;
	}
	
	scroll(); //scroll if needed
	move_cursor(); //incr cursor loc
}

//clear the screen
void monitor_clear()
{
	//attrib byte for def colors
	uint8_t colorattrib = (0 << 4) | (15 & 0x0F);
	uint16_t blank = 0x20 | (colorattrib << 8);
	
	int i;
	for (i = 0; i < 80*25; i++)
	{
		video_memory[i] = blank;
	}
	
	//move hw cursor back to the start
	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}

//print a null-terminated string to the screen
void monitor_write(char *c)
{
	int i = 0;
	while (c[i])
	{
		monitor_put(c[i++]);
	}
}

void monitor_write_hex(uint32_t n)
{
	
}

void monitor_write_dec(uint32_t n)
{
	char buf[10];

	memset(buf, '\0', 10);
	
	int i = 0;
	
	while ((i < 10 && n > 0) || (i == 0 && n == 0))
	{
		i++;
		buf[i] = (n % 10) + 48;
		n /= 10;
	}

	i = 9;
	
	while (i >= 0)
	{
		if (buf[i])
			monitor_put(buf[i]);
		i--;
	}
}
