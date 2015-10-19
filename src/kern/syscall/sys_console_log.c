/* This file is a part of the RazOS project
 *
 * sys_console_log.h -- sys_console_log() syscall
 *
 * Razbit 2015 */

#include <sys/types.h>

#include <console.h>

#include "syscall.h"
#include <api/razos.h>

uint32_t sys_console_log(struct registers_t* regs)
{
	if (valid_user_buffer(REG_ARG1(regs), REG_ARG2(regs)))
	{
		kprintf("%.*s", (const char*)REG_ARG1(regs), (uint32_t)REG_ARG2(regs));
		return 0;
	}
	else
	{
		return -EFAULT;
	}
}
