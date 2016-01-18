/* This file is a part of the RazOS project
 *
 * sys_execve.h -- execve() -syscall
 *
 * Razbit 2016 */

#ifndef SYS_EXECVE_H
#define SYS_EXECVE_H

#include <sys/types.h>
#include "syscall.h"

uint32_t sys_execve(struct registers_t* regs);

#endif /* SYS_EXECVE_H */
