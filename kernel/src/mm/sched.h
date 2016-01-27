/* This file is a part of the RazOS project
 *
 * sched.h -- scheduler
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#ifndef SCHED_H
#define SCHED_H

#include <sys/types.h>

/* In task.s */
void sched_begin(uint32_t stack, uint32_t argv, uint32_t envp) __attribute__((__noreturn__));
void sched_switch();

/* In task.c */
void sched_halt();
void sched_cont();

#endif /* SCHED_H */
