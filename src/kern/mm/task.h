/* This file is a part of the RazOS project
 *
 * task.h -- multitasking
 *
 * Razbit 2015 */

#ifndef TASK_H
#define TASK_H

#include <sys/types.h>
#include "../interrupt/isr.h" /* struct register_t */

struct task_t
{
    pid_t pid; /* Process ID */
    /* edi, esi, ebp, esp, eip, eax, ebx, ecx, edx,
       ds, cs, ss, eflags, useresp */
    struct register_t* regs;
    struct page_directory_t* page_dir; /* Page directory */
    struct task_t* parent; /* Parent task */
    struct task_t* next; /* Next task in the linked list */
};

/* Initialize the tasking system */
void init_tasking();

/* Called in interrupt/timer.c */
void schedule(struct register_t* regs);

/* UNIX fork(): copy address space, spawn new process */
pid_t do_fork();

/* Return PID of the current process */
pid_t get_pid();

#endif /* TASK_H */

