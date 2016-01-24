/* This file is a part of the RazOS project
 *
 * sys_time.c -- syscalls needed for <time.h>
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <api/razos.h>
#include "syscall.h"

#include <time.h>

#include "sys_time.h"

uint32_t sys_time(struct registers_t* regs)
{
	(void)regs;
	return (uint32_t)time(NULL);
}
