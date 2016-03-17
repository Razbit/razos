/* sys_setup.c -- set kernel parameters */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
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

#include <sys/types.h>
#include <errno.h>

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
		errno = ENOSYS;
		return (uint32_t)-1;
	}
}
