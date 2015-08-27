/* This file is a part of the RazOS project
 *
 * timer.c -- the Programmable Interval Timer
 *
 * Razbit 2014 */

#include "timer.h"
#include "irq.h"
#include "isr.h" /* For register_t */

#include <sys/types.h>
#include <portio.h>

#include "../mm/task.h" /* schedule() */
#include "../kio.h"

uint32_t ticks = 0;

static void pit_callback(struct register_t* regs)
{
    ticks++;
    //   schedule();
}

void init_pit(uint32_t freq)
{
    kprintf("Initializing PIT at %d Hz\n", freq);
        
    /* Install IRQ handler for the PIT */
    install_irq_handler(IRQ0, &pit_callback);

    /* PIT's input clock is 1 193 180 Hz and it takes a divisor
     * to calculate the correct output clock. */
    uint32_t divisor = 1193180/freq;

    /* Send the command byte */
    outb(0x43, 0x36);

    /* Send divisor byte by byte */
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

uint32_t pit_get_ticks()
{
    return ticks;
}

void pit_set_ticks(uint32_t num)
{
    ticks = num;
}
