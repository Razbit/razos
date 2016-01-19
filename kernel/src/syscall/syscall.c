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
#include "sys_tasking.h" /* exit, sched_yield, fork, wait */
#include "sys_fs.h"      /* open, close, creat, read, write, lseek,
                          * fcntl, fstat */
#include "sys_setup.h"   /* setup kernel */
#include "sys_pipe.h"    /* pipe() */
#include "sys_uvm.h"     /* brk, sbrk */
#include "sys_execve.h"    /* execve */

/* Add the syscall here AND into razos_kernel_headerds/api/razos.h */
syscall_t syscall_table[] =
{
	[SYS_EXIT] = &sys_exit,
	[SYS_SCHED_YIELD] = &sys_sched_yield,
	[SYS_FORK] = &sys_fork,
	[SYS_WAIT] = &sys_wait,
	[SYS_READ] = &sys_read,
	[SYS_WRITE] = &sys_write,
	[SYS_OPEN] = &sys_open,
	[SYS_CLOSE] = &sys_close,
	[SYS_CREAT] = &sys_creat,
	[SYS_LSEEK] = &sys_lseek,
	[SYS_SETUP] = &sys_setup,
	[SYS_PIPE] = &sys_pipe,
	[SYS_BRK] = &sys_brk,
	[SYS_SBRK] = &sys_sbrk,
	[SYS_FCNTL] = &sys_fcntl,
	[SYS_EXECVE] = &sys_execve,
	[SYS_FSTAT] = &sys_fstat
};

/* Make sure buffer is available to user */
int valid_user_buffer(uint32_t ptr, size_t len)
{
	if ((void*)ptr == NULL)
		return 0;
	
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
