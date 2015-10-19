/* This file is a part of the RazOS project
 *
 * sys_fork.h -- sys_fork() syscall
 *
 * Razbit 2015 */

#ifndef SYS_FORK_H
#define SYS_FORK_H

#include <sys/types.h>

#include "syscall.h"
#include <api/razos.h>

uint32_t sys_fork(struct registers_t* regs);

#endif /* SYS_FORK_H */
