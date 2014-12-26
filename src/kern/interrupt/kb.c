/* This file is a part of the RazOS project
 *
 * kb.c -- Keyboard driver
 * 
 * Razbit 2014 
 */

/* This version is _very_ simple.. */

#include <stdint.h>
#include <portio.h>

#include "irq.h"
#include "isr.h"

#include "../kio.h"

const char sc_to_ascii[] =
{
	0, 0, /* escape */
	'1', '2', '3', '4',	'5', '6', '7', '8',	'9', '0', '-', '=',
	0, /* backspace */
	0x09, /* tab */
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	0, /* enter */
	0, /* lcontrol */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	0, /* lshift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	0, /* rshift */
	'*', /* numpad * */
	0, /* lalt */
	' ',
	0, /* capslock */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1 - F10 */
	0, /* numlock */
	0, /* scroll lock */
	/* next line is from numpad */
	'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.'
};

#define SCROLL_LOCK 0x0001
#define NUM_LOCK	0x0002
#define CAPS_LOCK	0x0004
#define SHIFT		0x0008
#define ALT			0x0010
#define CONTROL		0x0020
#define LSHIFT		0x0040
#define RSHIFT		0x0080
#define LALT 		0x0100
#define RALT 		0x0200
#define LCONTROL 	0x0400
#define RCONTROL 	0x0800
#define RELEASE 	0x1000
#define NUMPAD		0x2000

/* Handle a keyboard event */
static void kb_handler(struct register_t* regs)
{
    uint8_t scancode = inb(0x60); /* KB's data buffer */
	uint8_t ascii;
	static uint16_t flags;

	//kprintf("Scancode: 0x%X", scancode);

	if (scancode == 0xE0)
	{
		flags |= NUMPAD;
		return;
	}
	
	if (scancode & 0x80 == 0x80)
	{
		flags |= RELEASE;
		scancode &= 0x7F;
	}

	if (scancode <= 0x53)
		ascii = sc_to_ascii[scancode];
	else
		ascii = 0;

	if (flags & RELEASE)
	{
		kprintf("Released\n");
		if (flags & NUMPAD)
		{
			if (scancode == 0x1D)
			{
				flags &= ~RCONTROL;
				flags &= ~CONTROL;
			}
			else if (scancode == 0x38)
			{
				flags &= ~RALT;
				flags &= ~ALT;
			}
		}
		else
		{
			switch (scancode)
			{
			case 0x2A:
				flags &= ~LSHIFT;
				flags &= ~SHIFT;
				break;
			case 0x36:
				flags &= ~RSHIFT;
				flags &= ~SHIFT;
				break;
			case 0x1D:
				flags &= ~LCONTROL;
				flags &= ~CONTROL;
				break;
			case 0x38:
				flags &= ~LALT;
				flags &= ~ALT;
				break;
			}
		}
	}
	else
	{
		if (flags & NUMPAD)
		{
			if (scancode == 0x1D)
			{
				flags |= RCONTROL;
				flags |= CONTROL;
			}
			else if (scancode == 0x38)
			{
				flags |= RALT;
				flags |= ALT;
			}
		}
		else
		{
			switch (scancode)
			{
			case 0x45:
				flags ^= NUM_LOCK; /* flip num lock flag */
				break;
			case 0x46:
				flags ^= SCROLL_LOCK;
				break;
			case 0x3A:
				flags ^= CAPS_LOCK;
				break;
			case 0x2A:
				flags |= LSHIFT;
				flags |= SHIFT;
				break;
			case 0x36:
				flags |= RSHIFT;
				flags |= SHIFT;
				break;
			case 0x1D:
				flags |= LCONTROL;
				flags |= CONTROL;
				break;
			case 0x38:
				flags |= LALT;
				flags |= ALT;
				break;
			}
		}
	}

	kprintf("%c : 0x%14.14x\n", ascii, flags);

	flags &= ~NUMPAD;
}

void init_kb()
{
    kprintf("Initializing keyboard\n");

    /* Install the IRQ handler */
    install_irq_handler(IRQ1, &kb_handler);
}
