/* This file is a part of the RazOS project
 *
 * panic.h -- a crappy kernel panic
 *
 * Razbit 2014 */

#include <asm/system.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <console.h>
#include "mm/task.h"

#include <panic.h>

void panic(const char* msg, ...)
{
	cli();

	char str[1024];
	va_list va;
	va_start(va, msg);
	pid_t pid = 0;
	if (cur_task)
		pid = cur_task->pid;
	vsprintf(str, msg, va);
	kprintf("\nKernel panic (pid %i): %s\n", pid, str);

	va_end(va);

	for(;;);
}
