/* This file is a part of the RazOS project
 *
 * razos.h -- the RazOS API: syscall numbers
 *
 * Razbit 2015, 2016 */

#ifndef RAZOS_H
#define RAZOS_H

#include <stdint.h>

#define ENOSYS 1 /* No such syscall */
#define EFAULT 2 /* Bad address */

/* Syscalls */
#define SYS_EXIT 0
#define SYS_SCHED_YIELD 1
#define SYS_FORK 2
#define SYS_WAIT 3

#define SYS_READ 4
#define SYS_WRITE 5
#define SYS_OPEN 6
#define SYS_CLOSE 7
#define SYS_CREAT 8
#define SYS_LSEEK 9
#define SYS_FCNTL 10

#define SYS_SETUP 11

#define SYS_PIPE 12
#define SYS_BRK 13
#define SYS_SBRK 14

#define SYS_EXECVE 15

/* In arch/../crt0.s */
uint32_t __syscall0(uint32_t num);
uint32_t __syscall1(uint32_t num, uint32_t arg1);
uint32_t __syscall2(uint32_t num, uint32_t arg1, uint32_t arg2);
uint32_t __syscall3(uint32_t num, uint32_t arg1, uint32_t arg2,	\
                    uint32_t arg3);

/* For sys_setup */
#define SET_ERRNO_LOC 1

#endif /* RAZOS_H */
