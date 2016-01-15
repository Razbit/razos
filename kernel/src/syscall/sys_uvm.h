/* This file is a part of the RazOS project
 *
 * sys_uvm.h -- user virtual memory syscalls
 *
 * Razbit 2016 */

#ifndef SYS_UVM_H
#define SYS_UVM_H

#include <sys/types.h>
#include "syscall.h"

uint32_t sys_brk(struct registers_t* regs);
uint32_t sys_sbrk(struct registers_t* regs);

#endif /* SYS_UVM_H */
