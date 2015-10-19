/* This file is a part of the RazOS project
 *
 * sys_yield.h -- sys_yield() syscall
 *
 * Razbit 2015 */

#ifndef SYS_YIELD_H
#define SYS_YIELD_H

#include <sys/types.h>

#include "syscall.h"
#include <api/razos.h>

uint32_t sys_yield(struct registers_t* regs);

#endif /* SYS_YIELD_H */
