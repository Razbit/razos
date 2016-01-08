/* This file is a part of the RazOS project
 *
 * sys_setup.c -- set kernel parameters
 *
 * Razbit 2016 */

#include <sys/types.h>

#include <api/razos.h>
#include "syscall.h"

#include "../mm/task.h"

#include "sys_setup.h"

uint32_t sys_setup(struct registers_t* regs)
{
	uint32_t cmd = REG_ARG1(regs);
	switch (cmd)
	{
	case SET_ERRNO_LOC:
		cur_task->errno_loc = (int*)REG_ARG2(regs);
		return 0;
		break;
	default:
		return -ENOSYS;
	}
}
