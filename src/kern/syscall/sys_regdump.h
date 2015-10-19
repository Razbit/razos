/* This file is a part of the RazOS project
 *
 * sys_regdump.h -- sys_regdump() syscall
 *
 * Razbit 2015 */

#ifndef SYS_REGDUMP_H
#define SYS_REGDUMP_H

#include <sys/types.h>

#include "syscall.h"
#include <api/razos.h>

uint32_t sys_regdump(struct registers_t* regs);

#endif /* SYS_REGDUMP_H */
