/* This file is a part of the RazOS project
 *
 * sys_wait.h -- sys_wait() syscall
 *
 * Razbit 2015 */

#ifndef SYS_WAIT_H
#define SYS_WAIT_H

#include <sys/types.h>

#include "syscall.h"
#include <api/razos.h>

uint32_t sys_wait(struct registers_t* regs);

#endif /* SYS_WAIT_H */
