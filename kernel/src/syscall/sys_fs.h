/* sys_fs.h -- filesystem-related syscalls */

/* Copyright (c) 2015, 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

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
uint32_t sys_fstat(struct registers_t* regs);

#endif /* SYS_FS_H */
