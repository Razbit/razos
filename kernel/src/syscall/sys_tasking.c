/* This file is a part of the RazOS project
 *
 * sys_tasking.c -- tasking-related syscalls
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <panic.h>
#include <errno.h>

#include "../mm/task.h"
#include "../mm/sched.h"

#include <api/razos.h>
#include "syscall.h"

#include "sys_tasking.h"


uint32_t sys_exit(struct registers_t* regs)
{
	uint32_t status = REG_ARG1(regs);

	if (cur_task->pid == 1)
		panic("Init exited with status %d\n", status);

	task_kill(cur_task, status);

	sched_switch(); /* We should never return to this process */
	return 0; /* If we happen to get here somehow (makes gcc happy) */
}


uint32_t sys_fork(struct registers_t* regs)
{
	(void)regs;
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


uint32_t sys_wait(struct registers_t* regs)
{
	uint32_t* stat_loc = (uint32_t*)REG_ARG1(regs);

	/* Fail if there are no children */
	if (cur_task->children == 0)
	{
		errno = ECHILD;
		return (uint32_t)-1;
	}

	/* Wait for a child to die and destroy it and return */
again:
	if (cur_task->wait_queue)
	{
		struct task_t* child = cur_task->wait_queue;
		/* If there are multiple dead children, the next one can now
		 * be found from the parent's wait queue */
		cur_task->wait_queue = child->wait_queue;

		pid_t child_pid = child->pid;
		uint32_t child_status = child->exit_status;

		task_destroy(child);

		if (stat_loc)
			*stat_loc = child_status;

		return child_pid;
	}

	/* Put this task to sleep until woken up by the death of a child */
	cur_task->state = TASK_STATE_BLOCK_WAIT;
	sched_switch();

	goto again;
}


uint32_t sys_sched_yield(struct registers_t* regs)
{
	(void)regs;
	sched_switch();
	return 0;
}
