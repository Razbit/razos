/* This file is a part of the RazOS project
 *
 * close.c -- close() from unistd (POSIX)
 *
 * Razbit 2015 */

#include <unistd.h>
#include <stdint.h>

#include <api/razos.h>

int close(int fd)
{
	return (int)__syscall1(SYS_CLOSE, (uint32_t)fd);
}
