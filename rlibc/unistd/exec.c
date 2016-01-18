/* This file is a part of the RazOS project
 *
 * exec.c -- exec() -family
 *
 * Razbit 2016 */

#include <unistd.h>
#include <api/razos.h>

char** environ = NULL;

/* TODO: arguments, environ */
/* TODO: O_CLOEXEC (in the kernel side, not here) */
int execve(const char* path, char* const argv[], char* const envp[])
{
	return __syscall3(SYS_EXECVE, (uint32_t)path, (uint32_t)argv, \
	                  (uint32_t)envp);
}
