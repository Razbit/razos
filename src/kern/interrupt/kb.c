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

static void kb_handler(struct register_t* regs)
{
    /* Handle a keyboard event */
    unsigned char scancode;

    scancode = inb(0x60); /* KB's data buffer */

    if (scancode & 0x80)
    {
        /* The key was released.. */
        kprintf("The key 0x%X was released\n", scancode&0x7F);
    }
    else
    {
        /* The key was pressed.. */
        kprintf("The key 0x%X was pressed\n", scancode);
    }
}

void init_kb()
{
    kprintf("Initializing keyboard\n");

    /* Install the IRQ handler */
    install_irq_handler(IRQ1, &kb_handler);
}
