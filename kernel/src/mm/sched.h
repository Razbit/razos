/* This file is a part of the RazOS project
 *
 * sched.h -- scheduler
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#ifndef SCHED_H
#define SCHED_H

/* Both in task.s */
void sched_begin() __attribute__((__noreturn__));
void sched_switch();

#endif /* SCHED_H */
