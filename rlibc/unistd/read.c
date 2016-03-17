/* This file is a part of the RazOS project
 *
 * read.c -- read() from unistd (POSIX)
 *
 * Razbit 2015 */

#include <unistd.h>
#include <stdint.h>

#include <api/razos.h>

ssize_t read(int fd, void* buf, size_t size)
{
	return (ssize_t)__syscall3(SYS_READ, (uint32_t)fd, (uint32_t)buf, \
	                           (uint32_t)size);
}
