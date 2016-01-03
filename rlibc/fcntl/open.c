/* This file is a part of the RazOS project
 *
 * open.c -- open() from fcntl
 *
 * Razbit 2015 */

#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>

#include <api/razos.h>


int open(const char* name, int oflag, ...)
{
	mode_t mode = 0;
	va_list args;
	va_start(args, oflag);
	if (oflag & O_CREAT)
		mode = va_arg(args, mode_t);
	va_end(args);

	return (int)__syscall3(SYS_OPEN, (uint32_t)name, (uint32_t)oflag, (uint32_t)mode);
}

