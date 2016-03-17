/* This file is a part of the RazOS project
 *
 * sched.h -- scheduler
 *
 * Razbit 2015, 2016 */

#ifndef SCHED_H
#define SCHED_H

#include <sys/types.h>

/* In task.s */
void sched_begin(uint32_t stack, uint32_t argv, uint32_t envp);
void sched_switch();

/* In task.c */
void sched_halt();
void sched_cont();

#endif /* SCHED_H */
