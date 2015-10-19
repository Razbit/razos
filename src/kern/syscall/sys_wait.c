/* This file is a part of the RazOS project
 *
 * sys_wait.c -- sys_wait() syscall
 *
 * Razbit 2015 */

#include <sys/types.h>

#include "../mm/task.h"
#include "../mm/sched.h"

#include "syscall.h"
#include <api/razos.h>

#include "sys_wait.h"

uint32_t sys_wait(struct registers_t* regs)
{
	if (REG_ARG1(regs) != 0 \
	    && !valid_user_buffer(REG_ARG1(regs), sizeof(uint32_t)))
	{
		return -EFAULT;
	}

	uint32_t* stat_loc = (uint32_t*)REG_ARG1(regs);

again:
	if (cur_task->wait_queue.live.head)
	{
		struct task_t* child = cur_task->wait_queue.live.head;
		cur_task->wait_queue.live.head = child->wait_queue.dead.next;
		
		if (!cur_task->wait_queue.live.head)
			cur_task->wait_queue.live.tail = NULL;

		pid_t child_pid = child->pid;
		uint32_t child_status = child->exit_status;

		task_destroy(child);

		if (stat_loc)
			*stat_loc = child_status;

		return child_pid;
	}

	cur_task->state = TASK_STATE_BLOCK_WAIT;
	sched_switch();
	goto again;
}
