/* This file is a part of the RazOS project
 *
 * razos.h -- the RazOS API: syscall numbers
 *
 * Razbit 2015 */

#ifndef RAZOS_H
#define RAZOS_H

#include <stdint.h>

#define ENOSYS 1 /* No such syscall */
#define EFAULT 2 /* Bad address */

/* Syscalls */
#define SYS_EXIT 0
#define SYS_YIELD 1
#define SYS_FORK 2
#define SYS_WAIT 3

#define SYS_REGDUMP 4
#define SYS_CONSOLE_LOG 5

/* In arch/../crt0.s */
uint32_t __syscall0(uint32_t num);
uint32_t __syscall1(uint32_t num, uint32_t arg1);
uint32_t __syscall2(uint32_t num, uint32_t arg1, uint32_t arg2);
uint32_t __syscall3(uint32_t num, uint32_t arg1, uint32_t arg2,	\
                    uint32_t arg3);

#endif /* RAZOS_H */
