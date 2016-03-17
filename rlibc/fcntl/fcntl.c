/* This file is a part of the RazOS project
 *
 * fcntl.c -- File control options
 *
 * Razbit 2016 */

#include <stdint.h>
#include <fcntl.h>
#include <stdarg.h>
#include <api/razos.h>

int fcntl(int fd, int cmd, ...)
{
	uint32_t arg;
	va_list args;
	va_start(args, cmd);

	switch(cmd)
	{
	case F_DUPFD:
	case F_SETFD:
	case F_SETFL:
		arg = va_arg(args, uint32_t);
		break;
	default:
		arg = 0;
		break;
	}

	va_end(args);

	return (int)__syscall3(SYS_FCNTL, (uint32_t)fd, (uint32_t)cmd, \
	                  (uint32_t)arg);
}
