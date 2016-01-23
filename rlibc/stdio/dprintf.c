/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <stdio.h>
#include <stdarg.h>

int dprintf(int fildes, const char *restrict format, ...)
{
	va_list args;
	va_start(args, format);
    
	int ret = vdprintf(fildes, format, args);

	va_end(args);
	return ret;
}
