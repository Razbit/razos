/* This file is a part of the RazOS project
 *
 * sys_yield.c -- sys_yield() syscall
 *
 * Razbit 2015 */

#include <sys/types.h>

#include "../mm/sched.h"

#include "syscall.h"
#include <api/razos.h>

#include "sys_yield.h"

uint32_t sys_yield(struct registers_t* regs)
{
	sched_switch();
	return 0;
}
