/* This file is a part of the RazOS project
 *
 * fork.c -- POSIX fork()
 *
 * Razbit 2015 */

#include <api/razos.h>
#include <unistd.h>

pid_t fork(void)
{
	return __syscall0(SYS_FORK);
}
