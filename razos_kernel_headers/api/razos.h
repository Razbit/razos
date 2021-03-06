/* razos.h -- the RazOS API: syscall interface */

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

#ifndef RAZOS_H
#define RAZOS_H

#include <stdint.h>

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
#define SYS_FSTAT 11
#define SYS_SETUP 12
#define SYS_PIPE 13
#define SYS_BRK 14
#define SYS_SBRK 15
#define SYS_EXECVE 16
#define SYS_TIME 17

/* In arch/../crt0.s */
uint32_t __syscall0(uint32_t num);
uint32_t __syscall1(uint32_t num, uint32_t arg1);
uint32_t __syscall2(uint32_t num, uint32_t arg1, uint32_t arg2);
uint32_t __syscall3(uint32_t num, uint32_t arg1, uint32_t arg2,	\
                    uint32_t arg3);

/* For sys_setup */
#define SET_ERRNO_LOC 1
#define GET_USED_MEM 2

#endif /* RAZOS_H */
