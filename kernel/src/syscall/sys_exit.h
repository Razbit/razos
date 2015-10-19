/* This file is a part of the RazOS project
 *
 * sys_exit.h -- sys_exit() syscall
 *
 * Razbit 2015 */

#ifndef SYS_EXIT_H
#define SYS_EXIT_H

#include <sys/types.h>

#include "syscall.h"
#include <api/razos.h>

uint32_t sys_exit(struct registers_t* regs);

#endif /* SYS_EXIT_H */
