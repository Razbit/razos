/* This file is a part of the RazOS project
 *
 * task.c -- multitasking
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <asm/system.h>
#include <panic.h>

#include "paging.h"
#include "kmalloc.h"
#include "../kio.h"
#include "../interrupt/isr.h"

#include "task.h"

/* Currently running task */
volatile struct task_t* cur_task = 0;
/* The task list */
volatile struct task_t* task_queue = 0;

/* In paging.c */
extern struct page_directory_t *kernel_dir;
extern struct page_directory_t *cur_dir;

/* Counter for PIDs */
uint32_t next_pid = 1;

/* Initialize the tasking system */
void init_tasking()
{
    cli();
    
    kprintf("Initializing tasking\n");
    
    /* Initialize the first task (kernel) */
    cur_task = task_queue = kmalloc(sizeof(struct task_t));
    cur_task->pid = next_pid++;
    cur_task->regs = NULL;
    cur_task->page_dir = cur_dir;
    cur_task->parent = NULL;
    cur_task->next = NULL;

    sti();
}
int counter = 0;
/* Called in interrupt/timer.c */
void schedule(struct register_t* regs)
{
    /* Return if tasking is not yet initialized */
    if (!cur_task)
        return;
           
    /* Copy current context to the task struct */
    memcpy((void*)cur_task->regs, (void*)regs, sizeof(struct register_t));

    /* Get next task in queue */
    cur_task = cur_task->next;
    if (!cur_task)
        cur_task = task_queue;

    /* Setup execution context */
    memcpy((void*)regs, (void*)cur_task->regs, sizeof(struct register_t));
    switch_page_dir(cur_task->page_dir);
}

/* UNIX fork(): copy address space, spawn new process */
pid_t do_fork()
{
    struct task_t* parent_task = cur_task;
    
    /* Create new task */
    struct task_t* new_task = kmalloc(sizeof(struct task_t));

    /* Clone address space */
    struct page_directory_t* dir = clone_page_dir(cur_dir);

    new_task->pid = next_pid++;
    new_task->page_dir = dir;
    new_task->parent = cur_task;
    new_task->next = NULL;

    /* Add to task queue */
    struct task_t* tmp = task_queue;
    while(tmp->next)
        tmp = tmp->next;
    tmp->next = new_task;

    cli();
    /* Read current execution context */
    uint32_t esp, ebp, edi, esi, eax, ebx, ecx, edx;
    uint32_t ds, cs, ss, eflags, eip;
    __asm__ __volatile__("mov %%edi, %0" : "=r"(edi));
    __asm__ __volatile__("mov %%esi, %0" : "=r"(esi));
    __asm__ __volatile__("mov %%ebp, %0" : "=r"(ebp));
    __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
    __asm__ __volatile__("mov %%ebx, %0" : "=r"(ebx));
    __asm__ __volatile__("mov %%edx, %0" : "=r"(edx));
    __asm__ __volatile__("mov %%ecx, %0" : "=r"(ecx));
    __asm__ __volatile__("mov %%eax, %0" : "=r"(eax));
    __asm__ __volatile__("mov %%ds, %0" : "=r"(ds));
    __asm__ __volatile__("mov %%cs, %0" : "=r"(cs));
    __asm__ __volatile__("mov %%ss, %0" : "=r"(ss));
    __asm__ __volatile__("pushf");
    __asm__ __volatile__("pop %0" : "=r"(eflags));
    cur_task->regs->esp = new_task->regs->esp = esp;
    cur_task->regs->useresp = new_task->regs->useresp = esp;
    cur_task->regs->ebp = new_task->regs->ebp = ebp;
    cur_task->regs->edi = new_task->regs->edi = edi;
    cur_task->regs->esi = new_task->regs->esi = esi;
    cur_task->regs->eax = new_task->regs->eax = eax;
    cur_task->regs->ebx = new_task->regs->ebx = ebx;
    cur_task->regs->ecx = new_task->regs->ecx = ecx;
    cur_task->regs->edx = new_task->regs->edx = edx;
    cur_task->regs->ds = new_task->regs->ds = ds;
    cur_task->regs->cs = new_task->regs->cs = cs;
    cur_task->regs->ss = new_task->regs->ss = ss;
    cur_task->regs->eflags = new_task->regs->eflags = eflags;
    
    /* This is where the new task starts executing */
    __asm__ __volatile__("mov $., %0" : "=r"(eip));

    kprintf("Forking\n");
    if (cur_task == parent_task)
    {
        cur_task->regs->eip = new_task->regs->eip = eip;
        sti();
        return new_task->pid;
    }
    else
    {
        return 0;
    }
    
}

/* Return PID of the current process */
pid_t get_pid()
{
    return cur_task->pid;
}

