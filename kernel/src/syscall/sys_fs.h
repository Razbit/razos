/* This file is a part of the RazOS project
 *
 * sys_fs.h -- filesystem-related syscalls
 *
 * Razbit 2015, 2016 */

#ifndef SYS_FS_H
#define SYS_FS_H

#include <sys/types.h>

#include <api/razos.h>
#include "syscall.h"

uint32_t sys_read(struct registers_t* regs);
uint32_t sys_write(struct registers_t* regs);
uint32_t sys_open(struct registers_t* regs);
uint32_t sys_close(struct registers_t* regs);
uint32_t sys_creat(struct registers_t* regs);
uint32_t sys_lseek(struct registers_t* regs);
uint32_t sys_fcntl(struct registers_t* regs);

#endif /* SYS_FS_H */
