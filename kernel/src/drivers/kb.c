/* kb.c -- Keyboard driver */

/* Copyright (c) 2014 Eetu "Razbit" Pesonen
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

/* see refs/kb_ref.txt for scan codes */

#include <sys/types.h>
#include <portio.h>
#include <console.h>
#include <ctype.h>

#include <fcntl.h>
#include <unistd.h>
#include "../mm/task.h"

#include "kb.h"

static const char sc_to_ascii[] =
{
	0, 0, /* escape */
	'1', '2', '3', '4',	'5', '6', '7', '8',	'9', '0', '-', '=',
	0, /* backspace */
	0x09, /* tab */
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	'\n', /* enter */
	0, /* lcontrol */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	0, /* lshift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
	0, /* rshift */
	'*', /* numpad * */
	0, /* lalt */
	' '
};

/* Modify ascii to incorporate effects of pressing shift.
 * Table begins at 0x27, ' -> " */
static const char shift_lookup[] =
{
	'"', 0, 0, 0, 0, '<', '_', '>', '?',    /* ' .. / */
	')', '!', '@', '#', '$', '%', '^', '&', /* 0 .. 7 */
	'*', '(', 0, ':', 0, '+', 0, 0,         /* 8 .. ? */
	0, 0, 0, 0, 0, 0, 0, 0,                 /* @ .. G */
	0, 0, 0, 0, 0, 0, 0, 0,                 /* H .. O */
	0, 0, 0, 0, 0, 0, 0, 0,                 /* P .. W */
	0, 0, 0, '{', '|', '}', 0, 0,           /* X .. _ */
	'~', 'A', 'B', 'C', 'D', 'E', 'F', 'G', /* ` .. g */
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', /* h .. o */
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', /* p .. w */
	'X', 'Y', 'Z'                           /* x .. z */
};

/* tempfl */
#define TFL_E0 0x01
#define TFL_E1 0x02
#define TFL_1D 0x04
#define TFL_2A 0x08
#define TFL_45 0x10
#define TFL_9D 0x20

static int stdin_fd;

int init_kb()
{
	stdin_fd = open("stdin", O_WRONLY);
	return stdin_fd;
}

/* Handle a keyboard event */
void kb_handler()
{
	uint8_t scancode = inb(0x60); /* KB's data buffer */
	uint8_t ascii = 0;
	uint8_t keycode = 0;
	static uint16_t flags = 0;
	static uint8_t tempfl;

	if (scancode == 0xE0)
	{
		tempfl |= TFL_E0;
		return;
	}

	if (scancode == 0xE1)
	{
		tempfl |= TFL_E1;
		return;
	}

	if (scancode & 0x80)
	{
		flags |= RELEASE;
	}

	/* Modifier keys */
	switch (scancode)
	{
	case 0x1C:
		ascii = '\n';
		/* fallthrough: key release won't send ascii code */
	case 0x9C:
		if (tempfl & TFL_E0)
		{
			flags |= NUMPAD;
			keycode = NUM_RET;
		}
		else
		{
			keycode = KEY_RET;
		}
		break;

	case 0x1D:
		if (tempfl & TFL_E1) /* E1, 1D */
		{
			tempfl |= TFL_1D;
			return;
		}
		if (tempfl & TFL_E0) /* E0, 1D -> rctrl */
		{
			flags |= RCONTROL;
			keycode = KEY_RCTRL;
		}
		else /* 1D -> lctrl */
		{
			flags |= LCONTROL;
			keycode = KEY_LCTRL;
		}
		break;

	case 0x9D:
		/* part of pause-sequence */
		if ((tempfl & (TFL_E1 | TFL_1D | TFL_45)) == \
		    (TFL_E1 | TFL_1D | TFL_45))
		{
			tempfl |= TFL_9D;
			return;
		}
		else /* ctrl release */
		{
			if (tempfl & TFL_E0)
			{
				flags &= ~RCONTROL;
				keycode = KEY_RCTRL;
			}
			else
			{
				flags &= ~LCONTROL;
				keycode = KEY_LCTRL;
			}
			flags &= ~CONTROL;
		}
		break;

	case 0x2A:
		if (tempfl & TFL_E0) /* part of print screen -sequence */
		{
			tempfl |= TFL_2A;
			return;
		}
		else
		{
			flags |= LSHIFT;
			keycode = scancode;
		}
		break;

	case 0xAA: /* lshift release */
		flags &= ~LSHIFT;
		flags &= ~SHIFT;
		keycode = KEY_LSHIFT;
		break;

	case 0x35:
		if (tempfl & TFL_E0)
		{
			keycode = NUM_SLASH;
			flags |= NUMPAD;
			ascii = '/';
		}
		else
		{
			keycode = KEY_SLASH;
			ascii = '/';
		}
		break;

	case 0x36:
		flags |= RSHIFT;
		keycode = KEY_RSHIFT;
		break;

	case 0xB6:
		flags &= ~RSHIFT;
		flags &= ~SHIFT;
		keycode = KEY_RSHIFT;
		break;

	case 0x37:
		if ((tempfl & TFL_E0) && (tempfl & TFL_2A)) /* E0 2A E0 37 */
		{
			keycode = KEY_PRSCR;
		}
		else
		{
			keycode = NUM_ASTER;
			flags |= NUMPAD;
			ascii = '*';
		}
		break;

	case 0x38:
		if (tempfl & TFL_E0)
		{
			flags |= RALT;
			keycode = KEY_RALT;
		}
		else
		{
			flags |= LALT;
			keycode = KEY_LALT;
		}
		break;

	case 0xB8:
		if (tempfl & TFL_E0)
		{
			flags &= ~RALT;
			keycode = KEY_RALT;
		}
		else
		{
			flags &= ~LALT;
			keycode = KEY_LALT;
		}
		flags &= ~ALT;
		break;

	case 0x3A:
		flags ^= CAPS_LOCK; /* Toggle caps lock */
		keycode = KEY_CAPSL;
		break;

	case 0x45:
		if ((tempfl & TFL_E1) && (tempfl & TFL_1D))
		{
			tempfl |= TFL_45;
			return;
		}
		else /* Toggle num lock */
		{
			flags ^= NUM_LOCK;
			keycode = KEY_NUML;
		}
		break;

	case 0xC5:
		if ((tempfl & (TFL_E1 | TFL_1D | TFL_45 | TFL_9D)) == \
		    (TFL_E1 | TFL_1D | TFL_45 | TFL_9D))
		{
			keycode = KEY_PAUSE;
		}
		break;

	case 0x46:
		flags ^= SCROLL_LOCK; /* Toggle scroll lock */
		keycode = KEY_SCRL;
		break;

	case 0x47:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_HOME;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_7;
			ascii = '7';
		}
		break;

	case 0x48:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_UP;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_8;
			ascii = '8';
		}
		break;

	case 0x49:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_PGUP;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_9;
			ascii = '9';
		}
		break;

	case 0x4A:
		flags |= NUMPAD;
		keycode = NUM_MINUS;
		ascii = '-';
		break;

	case 0x4B:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_LEFT;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_4;
			ascii = '4';
		}
		break;

	case 0x4C:
		flags |= NUMPAD;
		keycode = NUM_5;
		ascii = '5';
		break;

	case 0x4D:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_RIGHT;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_6;
			ascii = '6';
		}
		break;

	case 0x4E:
		flags |= NUMPAD;
		keycode = NUM_PLUS;
		ascii = '+';
		break;

	case 0x4F:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_END;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_1;
			ascii = '1';
		}
		break;

	case 0x50:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_DOWN;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_2;
			ascii = '2';
		}
		break;

	case 0x51:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_PGDOWN;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_3;
			ascii = '3';
		}
		break;

	case 0x52:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_INS;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_0;
			ascii = '0';
		}
		break;

	case 0x53:
		if ((tempfl & TFL_E0) || !(flags & NUM_LOCK))
		{
			keycode = KEY_DEL;
			ascii = 0x7F;
		}
		else
		{
			flags |= NUMPAD;
			keycode = NUM_DOT;
			ascii = '.';
		}
		break;

	default:
		/* Get the ASCII code for the key, if available */
		if (scancode <= 0x39)
			ascii = sc_to_ascii[scancode];
		else
			ascii = 0;

		keycode = scancode;
	}


	/* Set the rest of the flags */
	if ((flags & RSHIFT) || (flags & LSHIFT))
		flags |= SHIFT;
	if ((flags & RALT) || (flags & LALT))
		flags |= ALT;
	if ((flags & RCONTROL) || (flags & LCONTROL))
		flags |= CONTROL;

	/* Construct the packet */
	uint32_t packet = (uint32_t)flags;
	packet <<= 8;
	packet |= (uint32_t)keycode;
	packet <<= 8;
	packet |= (uint32_t)ascii;

	/* Make ascii code to match the flags */
	if ((ascii > 0) && (ascii < 0x7F))
	{
		/* If shift is pressed, modify the ascii */
		if ((ascii >= 0x27) && (flags & SHIFT) && !(flags & NUMPAD))
		{
			ascii = shift_lookup[ascii-0x27];
		}
		if (flags & CAPS_LOCK)
		{
			if (islower(ascii))
				ascii = toupper(ascii);
			else if (isupper(ascii))
				ascii = tolower(ascii);
		}
	}

	if (ascii > 0)
		write(stdin_fd, &ascii, 1);

	tempfl = 0;
	flags &= ~NUMPAD;
	flags &= ~RELEASE;
}
