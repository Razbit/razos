/* This file is a part of the RazOS project
 *
 * exec.c -- exec() -family
 *
 * Razbit 2016 */

#include <unistd.h>
#include <api/razos.h>

/* TODO: arguments, environ */
/* TODO: O_CLOEXEC (in the kernel side, not here) */
int exec(const char* path)
{
	return __syscall1(SYS_EXEC, (uint32_t)path);
}
