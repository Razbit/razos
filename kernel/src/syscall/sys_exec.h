/* This file is a part of the RazOS project
 *
 * sys_exec.h -- exec() -syscall
 *
 * Razbit 2016 */

#ifndef SYS_EXEC_H
#define SYS_EXEC_H

#include <sys/types.h>
#include "syscall.h"

uint32_t sys_exec(struct registers_t* regs);

#endif /* SYS_EXEC_H */
