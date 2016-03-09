/* This file is a part of the RazOS project
 *
 * task.h -- tasking and switching
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

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
