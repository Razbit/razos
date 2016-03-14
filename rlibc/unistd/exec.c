/* This file is a part of the RazOS project
 *
 * exec.c -- exec() -family
 *
 * Razbit 2016 */

#include <unistd.h>
#include <api/razos.h>

extern char** environ;

int execve(const char* path, char* const argv[], char* const envp[])
{
	return __syscall3(SYS_EXECVE, (uint32_t)path, (uint32_t)argv, \
	                  (uint32_t)envp);
}

int execv(const char* path, char* const argv[])
{
	return execve(path, argv, environ);
}
