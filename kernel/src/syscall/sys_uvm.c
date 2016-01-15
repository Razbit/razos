/* This file is a part of the RazOS project
 *
 * sys_uvm.c -- user virtual memory syscalls
 *
 * Razbit 2016 */

#include <sys/types.h>

#include <api/razos.h>
#include "syscall.h"

#include "../mm/uvm.h"

uint32_t sys_brk(struct registers_t* regs)
{
	return (uint32_t)uvm_brk((void*)REG_ARG1(regs));
}

uint32_t sys_sbrk(struct registers_t* regs)
{
	return (uint32_t)uvm_sbrk((intptr_t)REG_ARG1(regs));
}
