/* This file is a part of the RazOS project
 *
 * sys_console_log.h -- sys_console_log() syscall
 *
 * Razbit 2015 */

#ifndef SYS_CONSOLE_LOG_H
#define SYS_CONSOLE_LOG_H

#include <sys/types.h>

#include "syscall.h"
#include <api/razos.h>

uint32_t sys_console_log(struct registers_t* regs);

#endif /* SYS_CONSOLE_LOG_H */
