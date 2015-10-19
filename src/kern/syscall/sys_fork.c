/* This file is a part of the RazOS project
 *
 * sys_fork.c -- sys_fork() syscall
 *
 * Razbit 2015 */

#include <sys/types.h>

#include "../mm/sched.h"
#include "../mm/task.h"

#include "syscall.h"
#include <api/razos.h>

#include "sys_fork.h"

uint32_t sys_fork(struct registers_t* regs)
{
	struct task_t* new_task = task_fork();

	if (new_task)
	{
		/* We are the parent */
		return new_task->pid;
	}
	else
	{
		/* We are the child */
		return 0;
	}
}
