/* This file is a part of the RazOS project
 *
 * isr.c -- interrupt handling
 *
 * Razbit 2014 */

#include "isr.h"

#include "kio.h"

/* Called from asm */
void isr_handler(struct register_t regs)
{
    kprintf("Received interrupt %d with error code %d\n", regs.int_no,\
        regs.err_code);
}
