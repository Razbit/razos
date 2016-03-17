/* sys_tasking.c -- tasking-related syscalls */

/* Copyright (c) 2015, 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

/* Parts of this file are based on the work of Charlie Somerville.
 * The original file can be obtained here:
 * https://github.com/charliesome/radium
 *
 * Copyright (c) 2013-2015 Charlie Somerville
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#include <sys/types.h>
#include <panic.h>
#include <errno.h>

#include "../mm/task.h"
#include "../mm/sched.h"

#include <api/razos.h>
#include "syscall.h"

#include "sys_tasking.h"

#include <asm/power.h>

uint32_t sys_exit(struct registers_t* regs)
{
	uint32_t status = REG_ARG1(regs);

	if (cur_task->pid == 1)
		shutdown();

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
