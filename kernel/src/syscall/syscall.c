/* syscall.c -- syscall interface */

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
#include <util.h>
#include <errno.h>
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
#include "sys_execve.h"  /* execve */
#include "sys_time.h"    /* time() */

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
	[SYS_FSTAT] = &sys_fstat,
	[SYS_TIME] = &sys_time
};

/* Dispatch a syscall. Called in syscall.s */
void syscall_dispatch(struct registers_t* regs)
{
	if (REG_VECTOR(regs) > countof(syscall_table))
	{
		errno = ENOSYS;
		REG_RETURN(regs) = (uint32_t)-1;
		return;
	}

	syscall_t func = syscall_table[REG_VECTOR(regs)];

	if (!func)
	{
		errno = ENOSYS;
		REG_RETURN(regs) = (uint32_t)-1;
		return;
	}

	cur_task->syscall_regs = regs;

	REG_RETURN(regs) = func(regs);

	cur_task->syscall_regs = NULL;
}
