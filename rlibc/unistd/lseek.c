/* This file is a part of the RazOS project
 *
 * lseek.c -- lseek from unistd (POSIX)
 *
 * Razbit 2015 */

#include <unistd.h>
#include <stdint.h>

#include <api/razos.h>

off_t lseek(int fd, off_t offset, int whence)
{
	return (off_t)__syscall3(SYS_LSEEK, (uint32_t)fd, \
	                         (uint32_t)offset, (uint32_t)whence);
}

