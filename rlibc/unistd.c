/* This file is a part of the RazOS project
 *
 * unistd.h -- POSIX standard constants and types
 *
 * Razbit 2015 */

#include "unistd.h"
#include "stdint.h"

#include <api/razos.h>

off_t lseek(int fd, off_t offset, int whence)
{
	return (off_t)__syscall3(SYS_LSEEK, (uint32_t)fd, \
	                         (uint32_t)offset, (uint32_t)whence);
}

ssize_t read(int fd, void* buf, size_t size)
{
	return (ssize_t)__syscall3(SYS_READ, (uint32_t)fd, (uint32_t)buf, \
	                           (uint32_t)size);
}

ssize_t write(int fd, const void* buf, size_t size)
{
	return (ssize_t)__syscall3(SYS_WRITE, (uint32_t)fd, (uint32_t)buf,\
	                           (uint32_t)size);
}

int close(int fd)
{
	return (int)__syscall1(SYS_CLOSE, (uint32_t)fd);
}
