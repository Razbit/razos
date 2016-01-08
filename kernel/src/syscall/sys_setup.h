/* This file is a part of the RazOS project
 *
 * sys_setup.h -- set kernel parameters
 *
 * Razbit 2016 */

#ifndef SYS_SETUP_H
#define SYS_SETUP_H

#include <sys/types.h>

#include <api/razos.h>
#include "syscall.h"

uint32_t sys_setup(struct registers_t* regs);

#endif /* SYS_SETUP_H */
