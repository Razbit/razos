/* This file is a part of the RazOS project
 *
 * brk.c -- brk() and sbrk() syscalls
 *
 * Razbit 2016 */

#include <unistd.h>
#include <stdint.h>

#include <api/razos.h>

int brk(void* addr)
{
	return (int)__syscall1(SYS_BRK, (uint32_t)addr);
}

void* sbrk(intptr_t incr)
{
	return (void*)__syscall1(SYS_SBRK, (uint32_t)incr);
}
