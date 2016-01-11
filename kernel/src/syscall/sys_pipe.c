/* This file is a part of the RazOS project
 *
 * sys_pipe.c -- the pipe() syscall
 *
 * Razbit 2016 */

#include <sys/types.h>

#include <api/razos.h>
#include "syscall.h"

#include "../fs/pipe.h"

uint32_t sys_pipe(struct registers_t* regs)
{
	return (uint32_t)creat_pipe((int*)REG_ARG1(regs));
}
