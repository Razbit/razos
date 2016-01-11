/* This file is a part of the RazOS project
 *
 * sys_pipe.h -- the pipe() syscall
 *
 * Razbit 2016 */

#ifndef SYS_PIPE_H
#define SYS_PIPE_H

#include <sys/types.h>

#include <api/razos.h>
#include "syscall.h"

uint32_t sys_pipe(struct registers_t* regs);

#endif /* SYS_PIPE_H */
