/* This file is a part of the RazOS project
 *
 * sys_execve.c -- execve() -syscall
 *
 * Razbit 2016 */

#include <sys/types.h>

#include "syscall.h"
#include "sys_execve.h"
#include "../loader/exec.h"
#include "../mm/paging.h"

/* Returns execution at 0x10000000 when succesful */
uint32_t sys_execve(struct registers_t* regs)
{
	char* path = (char*)REG_ARG1(regs);
	char** argv = (char**)REG_ARG2(regs);
	char** envp = (char**)REG_ARG3(regs);

	uint32_t* args = execve(path, argv, envp);

	if (args == NULL)
		return -1;

	REG_ARG2(regs) = args[1];
	REG_ARG3(regs) = args[0];

	regs->ecx = args[2];
	regs->edx = UCODE_BEGIN;

	return 0;
}
