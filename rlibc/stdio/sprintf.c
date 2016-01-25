/* This file is a part of the RazOS project
 *
 * sprintf.c -- sprintf()
 *
 * Razbit 2016 */

#include <stdio.h>
#include <stdarg.h>

int sprintf(char* buf, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int ret = vsprintf(buf, fmt, args);

	va_end(args);
	return ret;
}
