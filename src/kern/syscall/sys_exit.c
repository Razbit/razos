/* This file is a part of the RazOS project
 *
 * sys_exit.c -- sys_exit() syscall
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <panic.h>
#include "../mm/task.h"
#include "../mm/sched.h"

#include "syscall.h"
#include <api/razos.h>

#include "sys_exit.h"

uint32_t sys_exit(struct registers_t* regs)
{
	uint32_t status = REG_ARG1(regs);

	if (cur_task->pid == 1)
		panic("Init exited with status %d\n", status);

	task_kill(cur_task, status);

	sched_switch();

	return 0;
}
