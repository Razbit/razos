/* This file is a part of the RazOS project
 *
 * syscall.h -- syscall interface
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#ifndef SYSCALL_H
#define SYSCALL_H

#include <sys/types.h>

struct registers_t
{
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
};

/* Macros for fetching information passed in the registers */
#define REG_VECTOR(regs) ((regs)->eax)
#define REG_RETURN(regs) ((regs)->eax)
#define REG_ARG1(regs) ((regs)->ebx)
#define REG_ARG2(regs) ((regs)->edi)
#define REG_ARG3(regs) ((regs)->esi)

/* Syscall function typedef */
typedef uint32_t (*syscall_t)(struct registers_t*);

extern syscall_t syscall_table[];

/* Initalize syscall interface (syscall.s) */
void syscall_init();

#endif /* SYSCALL_H */
