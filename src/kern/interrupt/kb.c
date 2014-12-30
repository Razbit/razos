/* This file is a part of the RazOS project
 *
 * kb.c -- Keyboard driver
 * 
 * Razbit 2014 
 */

#include <sys/types.h>
#include <portio.h>

#include "irq.h"
#include "isr.h"
#include "kb.h"

#include "../kio.h"

const char sc_to_ascii[] =
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
#define PAUSE       0x4000

/* Handle a keyboard event */
static void kb_handler(struct register_t* regs)
{
    uint8_t scancode = inb(0x60); /* KB's data buffer */
	uint8_t ascii = 0;
    uint8_t keycode = 0;
	static uint16_t flags;

	if (scancode == 0xE0)
	{
		flags |= NUMPAD;
		return;
	}

    if (scancode == 0xE1)
    {
        flags |= PAUSE;
        return;
    }

    if (scancode & 0x80)
        flags |= RELEASE;
	
	/* Modifier keys */
    switch (scancode)
    {
    case 0x1D:
        if (flags & PAUSE)
            return;
        if (flags & NUMPAD)
        {
            flags |= RCONTROL;
            keycode = KEY_RCTRL;
        }
        else
        {
            flags |= LCONTROL;
            keycode = scancode;
        }
        break;
    case 0x9D:
        if (flags & PAUSE)
            return;
        if (flags & NUMPAD)
        {
            flags &= ~RCONTROL;
            keycode = KEY_RCTRL;
        }
        else
        {
            flags &= ~LCONTROL;
            keycode = scancode;
        }
        flags &= ~CONTROL; break;
        
    case 0x2A:
        flags |= LSHIFT;
        keycode = scancode; break;
    case 0x36:
        flags |= RSHIFT;
        keycode = scancode; break;
    case 0xAA:
        flags &= ~LSHIFT;
        flags &= ~SHIFT;
        keycode = scancode; break;
    case 0xB6:
        flags &= ~RSHIFT;
        flags &= ~SHIFT;
        keycode = scancode; break;
        
    case 0x38:
        if (flags & NUMPAD)
        {
            flags |= RALT;
            keycode = KEY_RALT;
        }
        else
        {
            flags |= LALT;
            keycode = scancode;
        }
        break;
    case 0xB8:
        if (flags & NUMPAD)
        {
            flags &= ~RALT;
            keycode = KEY_RALT;
        }            
        else
        {
            flags &= ~LALT;
            keycode = scancode;
        }
        flags &= ~ALT; break;

    case 0x37:
        if (flags & NUMPAD)
            keycode = KEY_PRSCR;
        else
        {
            ascii = '*';
            keycode = NUM_ASTER;
        }
        break;
    case 0xB7:
        if (flags & NUMPAD)
            keycode = KEY_PRSCR;
        else
        {
            ascii = '*';
            keycode = NUM_ASTER;
        }
        break;            
            
    case 0x3A:
        flags ^= CAPS_LOCK;
        keycode = scancode; break;
    case 0x45:
        if (flags & PAUSE)
            return;
        flags ^= NUM_LOCK;
        keycode = scancode; break;
    case 0x46:
        flags ^= SCROLL_LOCK;
        keycode = scancode; break;
        
    case 0xC5:
        if (flags & PAUSE)
        {
            /* Pause was pressed.. God, what a series of scancodes! */
            keycode = KEY_PAUSE;
        }
        break;
       
    default:
        /* The ASCII code for the key, if available */
        if (scancode <= 0x53)
            ascii = sc_to_ascii[scancode];
        else
            ascii = 0;
    
        /* Numpad return */
        if ((scancode == 0x1C) && (flags & NUMPAD))
            ascii = '\n';
        keycode = scancode;
    }

    if (flags & RSHIFT || flags & LSHIFT)
        flags |= SHIFT;
    if (flags & RALT || flags & LALT)
        flags |= ALT;
    if (flags & RCONTROL || flags & LCONTROL)
        flags |= CONTROL;

    uint32_t packet = (uint32_t)flags;
    packet <<= 8;
    packet |= (uint32_t)keycode;
    packet <<= 8;
    packet |= (uint32_t)ascii;

    /* Send the kb event packet here */
    /* kprintf("%p\n", packet); */
    
	flags &= ~NUMPAD;
    flags &= ~PAUSE;
    flags &= ~RELEASE;
}

void init_kb()
{
    kprintf("Initializing keyboard\n");

    /* Install the IRQ handler */
    install_irq_handler(IRQ1, &kb_handler);
}
