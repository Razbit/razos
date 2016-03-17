/* This file is a part of the RazOS project
 *
 * panic.h -- a crappy kernel panic
 *
 * Razbit 2014, 2016 */

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
