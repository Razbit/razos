/* This file is a part of the RazOS project
 *
 * fcntl.h -- File control options
 *
 * Razbit 2015 */

#include <fcntl.h>
#include <sys/types.h>
#include <stdarg.h>

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
