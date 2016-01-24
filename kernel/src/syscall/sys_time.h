/* This file is a part of the RazOS project
 *
 * sys_time.h -- syscalls needed for <time.h>
 *
 * Razbit 2016 */

#ifndef SYS_TIME_H
#define SYS_TIME_H

#include <sys/types.h>
#include <api/razos.h>
#include "syscall.h"

uint32_t sys_time(struct registers_t* regs);

#endif /* SYS_TIME_H */
