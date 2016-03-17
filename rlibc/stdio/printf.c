/* This file is a part of the RazOS project
 *
 * printf.c -- printf()
 *
 * Razbit 2016 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

int printf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int ret = vdprintf(STDOUT_FILENO, fmt, args);

	va_end(args);
	return ret;
}
