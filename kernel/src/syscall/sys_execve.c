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

#include "syscall.h"
#include "sys_execve.h"
#include "../loader/exec.h"
#include "../mm/paging.h"

/* Returns execution at 0x10000000 when succesful */
uint32_t sys_execve(struct registers_t* regs)
{
	char* path = (char*)REG_ARG1(regs);
	char** argv = (char**)REG_ARG2(regs);
	char** envp = (char**)REG_ARG3(regs);

	uint32_t* args = execve(path, argv, envp);

	if (args == NULL)
		return -1;

	REG_ARG2(regs) = args[1];
	REG_ARG3(regs) = args[0];

	regs->ecx = args[2];
	regs->edx = UCODE_BEGIN;

	return 0;
}
