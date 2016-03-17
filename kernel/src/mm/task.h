/* task.c -- tasking */

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

/* Parts of this file are based on the work of Charlie Somerville.
 * The original file can be obtained here:
 * https://github.com/charliesome/radium
 *
 * Copyright (c) 2013-2015 Charlie Somerville
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

#ifndef TASK_H
#define TASK_H

#include <sys/types.h>
#include <asm/x86.h>
#include <limits.h>
#include "../syscall/syscall.h"
#include "../fs/vfs.h"

#include "paging.h"

/* Task state flags */
#define TASK_STATE_READY 1
#define TASK_STATE_BLOCK_WAIT 2
#define TASK_STATE_EXITING 3

/* In task.s we refer to hard-coded memory offsets within this,
 * so be careful when changing either */
struct task_t
{
	uint8_t fpu_state[512];      /* 0x0000 */
	uint32_t esp;                /* 0x0200 */
	uint32_t eip;                /* 0x0204 */
	struct page_dir_t* page_dir; /* 0x0208 */

	struct registers_t* syscall_regs;

	pid_t pid;
	pid_t ppid;

	uint32_t state;
	uint32_t exit_status;

	uint32_t children;           /* Number of children */

	struct task_t* wait_queue;   /* List of killed children */

	void* uheap_begin;           /* Beginning of user heap */
	void* uheap_end;             /* End of user heap */

	struct fildes_t files[OPEN_MAX]; /* Files used by the process */
	int* errno_loc;              /* errno is here */
};

extern struct task_t* cur_task;

/* Initialize tasking */
void task_init();

/* Internals of fork() begin here. In task.s */
struct task_t* task_fork();

/* Kill task */
void task_kill(struct task_t* task, uint32_t status);

/* Free resourses taken by the task */
void task_destroy(struct task_t* task);

struct task_t* get_task(pid_t pid);

#endif /* TASK_H */
