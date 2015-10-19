/* This file is a part of the RazOS project
 *
 * syscall.h -- syscall interface
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <util.h>
#include "../mm/paging.h"
#include "../mm/task.h"

#include "syscall.h"
#include <api/razos.h>

/* Include all syscall headers here */
#include "sys_exit.h"
#include "sys_yield.h"
#include "sys_fork.h"
#include "sys_wait.h"
#include "sys_regdump.h"
#include "sys_console_log.h"

/* Add the syscall here AND into <api/razos.h> */
syscall_t syscall_table[] =
{
	[SYS_EXIT] = &sys_exit,
	[SYS_YIELD] = &sys_yield,
	[SYS_FORK] = &sys_fork,
	[SYS_WAIT] = &sys_wait,
	[SYS_REGDUMP] = &sys_regdump,
	[SYS_CONSOLE_LOG] = &sys_console_log
};

/* Make sure buffer is available to user */
int valid_user_buffer(uint32_t ptr, size_t len)
{
	if ((0xFFFFFFFF - len) < ptr)
		return 0;

	size_t page_offset = ptr % PAGE_SIZE;
	ptr -= page_offset;
	len += page_offset;

	uint32_t end = ptr + len;
	while (ptr < end)
	{
		if (!page_mapped_to_user(ptr))
			return 0;
		ptr += PAGE_SIZE;
	}

	return 1;
}

/* Dispatch a syscall. Called in syscall.s */
void syscall_dispatch(struct registers_t* regs)
{
	if (REG_VECTOR(regs) > countof(syscall_table))
	{
		REG_RETURN(regs) = -ENOSYS;
		return;
	}

	syscall_t func = syscall_table[REG_VECTOR(regs)];

	if (!func)
	{
		REG_RETURN(regs) = -ENOSYS;
		return;
	}

	cur_task->syscall_regs = regs;

	REG_RETURN(regs) = func(regs);

	cur_task->syscall_regs = NULL;
}
