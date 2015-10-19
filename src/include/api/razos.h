/* This file is a part of the RazOS project
 *
 * razos.h -- the RazOS API: syscall numbers
 *
 * Razbit 2015 */

#ifndef RAZOS_H
#define RAZOS_H

#define ENOSYS 1 /* No such syscall */
#define EFAULT 2 /* Bad address */

/* Syscalls */
#define SYS_EXIT 0
#define SYS_YIELD 1
#define SYS_FORK 2
#define SYS_WAIT 3

#define SYS_REGDUMP 4
#define SYS_CONSOLE_LOG 5

#endif /* RAZOS_H */
