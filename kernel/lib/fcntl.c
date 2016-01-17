/* This file is a part of the RazOS project
 *
 * fcntl.c -- File control options
 *
 * Razbit 2015, 2016 */

#include <fcntl.h>
#include <sys/types.h>
#include <stdarg.h>
#include "../src/fs/vfs.h"

int open(const char* name, int oflag, ...)
{
	mode_t mode = 0;
	va_list args;
	va_start(args, oflag);
	if (oflag & O_CREAT)
		mode = va_arg(args, mode_t);
	va_end(args);

	return open_vfs(name, oflag, mode);
}

int creat(const char* name, mode_t mode)
{
	return creat_vfs(name, mode);
}

int close(int fd)
{
	return close_vfs(fd);
}

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

	return do_fcntl(fd, cmd, arg);
}
