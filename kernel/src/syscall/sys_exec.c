/* This file is a part of the RazOS project
 *
 * sys_exec.c -- exec() -syscall
 *
 * Razbit 2016 */

#include <sys/types.h>
#include "syscall.h"
#include "sys_exec.h"
#include "../loader/exec.h"
#include "../mm/paging.h"

/* Returns execution at 0x10000000 when succesful */
uint32_t sys_exec(struct registers_t* regs)
{
	char* path = (char*)REG_ARG1(regs);
	int ret = exec(path);
	if (ret < 0)
		return -1;
	
	regs->ecx = USER_STACK_END;
	regs->edx = USER_CODE_BEGIN;
	return (uint32_t)ret;
}
