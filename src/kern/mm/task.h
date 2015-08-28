/* This file is a part of the RazOS project
 *
 * task.h -- multitasking
 *
 * Razbit 2015 */

#ifndef TASK_H
#define TASK_H

#include <sys/types.h>

struct task_t
{
    pid_t pid;/* Process ID */
    uint32_t esp; /* Stack pointer */
    uint32_t ebp; /* Stack base pointer */
    uint32_t eip; /* Instruction pointer */
    struct page_directory_t *page_dir; /* Page directory */
    struct task *next; /* Next task in the linked list */
};

/* Initialize the tasking system */
void init_tasking();

/* Called in interrupt/timer.c */
void schedule();

/* UNIX fork(): copy address space, spawn new process */
pid_t do_fork();

/* Return PID of the current process */
pid_t get_pid();

void print_task(struct task_t* task);
void dump_task();

#endif /* TASK_H */

