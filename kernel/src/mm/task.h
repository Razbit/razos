/* This file is a part of the RazOS project
 *
 * task.h -- tasking and switching
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#ifndef TASK_H
#define TASK_H

#include <sys/types.h>
#include "../syscall/syscall.h"
#include "../fs/vfs.h"

#include "paging.h"

/* x86 Task State Segment */
struct tss_t
{
	uint16_t link;   /* 0x00 */
	uint16_t _res_1; 
	uint32_t esp0;   /* 0x04 */
	uint16_t ss0;    /* 0x08 */
	uint16_t _res_2; 
	uint32_t esp1;   /* 0x0C */
	uint16_t ss1;    /* 0x10 */
	uint16_t _res_3;
	uint32_t esp2;   /* 0x14 */
	uint16_t ss2;    /* 0x18 */
	uint16_t _res_4;
	uint32_t cr3;    /* 0x1C */
	uint32_t eip;    /* 0x20 */
	uint32_t eflags; /* 0x24 */
	uint32_t eax;    /* 0x28 */
	uint32_t ecx;    /* 0x2C */
	uint32_t edx;    /* 0x30 */
	uint32_t ebx;    /* 0x34 */
	uint32_t esp;    /* 0x38 */
	uint32_t ebp;    /* 0x3C */
	uint32_t esi;    /* 0x40 */
	uint32_t edi;    /* 0x44 */
	uint16_t es;     /* 0x48 */
	uint16_t _res_5;
	uint16_t cs;     /* 0x4C */
	uint16_t _res_6;
	uint16_t ss;     /* 0x50 */
	uint16_t _res_7;
	uint16_t ds;     /* 0x54 */
	uint16_t _res_8;
	uint16_t fs;     /* 0x58 */
	uint16_t _res_9;
	uint16_t gs;	 /* 0x5C */
	uint16_t _res_10;
	uint16_t ldtr;   /* 0x60 */
	uint16_t _res_11;
	uint16_t _res_12;
	uint16_t iopb;	 /* 0x66 */
}__attribute__((__packed__));

/* Task state flags */
#define TASK_STATE_READY 1
#define TASK_STATE_BLOCK_WAIT 2
#define TASK_STATE_EXITING 3

/* In task.s we refer to hard-coded memory offsets within this,
 * so be careful when changing either */
struct task_t
{
	uint8_t fpu_state[512]; /* 0x00 */
	uint32_t esp; /* 0x200 */
	uint32_t eip; /* 0x204 */
	void* kstack; /* 0x208 */
	uint32_t* page_dir; /* 0x20C */
	uint32_t page_dir_phys; /* 0x210 */

	/* Referring by offsets stops here, so do what you wish */
	struct registers_t* syscall_regs;

	pid_t pid;
	pid_t ppid;

	uint32_t state;
	uint32_t exit_status;

	union
	{
		struct
		{
			struct task_t* head;
			struct task_t* tail;
		} live;
		struct
		{
			struct task_t* next;
		} dead;
	} wait_queue;

	uint32_t stack_begin; /* Lowest address of the stack */
	struct fildes_t files[32]; /* Files used by the process */
	int* errno_loc; /* errno is here, set using
	                 * sys_setup(SET_ERRNO_LOC, errno) */
};

extern struct task_t* cur_task;

/* Initialize tasking */
void task_init();
void task_init_stdio();

/* Internals of fork() begin here. In task.s */
struct task_t* task_fork();

/* Kill task [task] */
void task_kill(struct task_t* task, uint32_t status);

/* Free resourses taken by the task */
void task_destroy(struct task_t* task);

struct task_t* get_task(pid_t pid);

#endif /* TASK_H */
