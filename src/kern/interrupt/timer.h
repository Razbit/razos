/* This file is a part of the RazOS project
 *
 * timer.h -- the Programmable Interval Timer
 *
 * Razbit 2014 */

#ifndef TIMER_H
#define TIMER_H

#include <sys/types.h>

void init_pit(uint32_t freq);
uint32_t pit_get_ticks();
void pit_set_ticks(uint32_t tick);

#endif /* TIMER_H */
