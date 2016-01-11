/* This file is a part of the RazOS project
 *
 * pipe.c -- pipe from unistd (POSIX)
 *
 * Razbit 2016 */

#include <unistd.h>
#include <stdint.h>

#include <api/razos.h>

int pipe(int fd[2])
{
	return (int)__syscall1(SYS_PIPE, (uint32_t)fd);
}
