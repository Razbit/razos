/* panic.c -- a crappy kernel panic */

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

#include <asm/system.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <console.h>
#include "mm/task.h"
#include "mm/sched.h"

#include <panic.h>

char str[1024];

void panic(const char* msg, ...)
{
	sched_halt();

	va_list va;
	va_start(va, msg);

	pid_t pid = 0;

	if (cur_task)
		pid = cur_task->pid;

	vsprintf(str, msg, va);
	kprintf("\nKernel panic (pid %i): %s\n", pid, str);

	va_end(va);

	sti();
	for(;;);
}
