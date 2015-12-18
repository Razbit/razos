/* This file is a part of the RazOS project
 *
 * sys_tasking.h -- tasking-related syscalls
 *
 * Razbit 2015 */

#ifndef SYS_TASKING_H
#define SYS_TASKING_H

#include <sys/types.h>

#include <api/razos.h>
#include "syscall.h"

uint32_t sys_exit(struct registers_t* regs);
uint32_t sys_fork(struct registers_t* regs);
uint32_t sys_wait(struct registers_t* regs);
uint32_t sys_sched_yield(struct registers_t* regs);

#endif /* SYS_TASKING_H */
