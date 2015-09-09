/* This file is a part of the RazOS project
 *
 * task.c -- multitasking
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <asm/system.h>
#include <panic.h>
#include <string.h>
#include <kassert.h>

#include "paging.h"
#include "stack.h"
#include "kmalloc.h"
#include "../kio.h"
#include "../interrupt/isr.h"

#include "task.h"

/* Currently running task */
volatile struct task_t* cur_task;
struct task_t* new_task;
/* The task list */
volatile struct task_t* task_queue;

/* In paging.c */
extern struct page_directory_t* kernel_dir;
extern struct page_directory_t* cur_dir;

/* In stack.c */
extern struct stack_t* kstack;
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
    cur_task->regs = kmalloc(sizeof(struct register_t));
    cur_task->stack = kstack;
    cur_task->page_dir = cur_dir;
    cur_task->parent = NULL;
    cur_task->next = NULL;

    sti();

    __asm__ __volatile__("int $32");
}
int counter = 0;
/* Called in interrupt/timer.c */
void schedule(struct register_t* regs)
{
    /* Return if tasking is not yet initialized */
    if (!cur_task)
        return;
    kprintf("Scheduling \n");

    /* Copy current context to the task struct */
    memcpy(cur_task->regs, regs, sizeof(struct register_t));
        
    /* Get next task in queue */
    cur_task = cur_task->next;
    if (!cur_task)
        cur_task = task_queue;

    switch_page_dir(cur_task->page_dir);

    /* This is the last-forked task (forking incomplete)
     * -> finalize forking */
    if (cur_task->regs == NULL)
    {
        kprintf("Finalizing forking: pid %u\n", get_pid());
        cur_task->regs = kmalloc(sizeof(struct register_t));
        memcpy(cur_task->regs, cur_task->parent->regs,  \
               sizeof(struct register_t));

        uint32_t stack_end =                                            \
            (uint32_t)cur_task->stack->start + cur_task->stack->size;
        cur_task->regs->esp = stack_end - cur_task->parent->stack->offset;
        
        kassert(cur_task == new_task);
    }
    else
    {
        uint32_t stack_end =                                            \
            (uint32_t)cur_task->stack->start + cur_task->stack->size;
        cur_task->stack->offset = stack_end - cur_task->regs->esp;
        
    }
    kprintf("SCHED: ESP: 0x%p EBP: 0x%p\n", cur_task->regs->esp, cur_task->regs->ebp);
    //dump_registers(cur_task->regs);

    /* Setup execution context */
    memcpy(regs, cur_task->regs, sizeof(struct register_t));
}

/* UNIX fork(): copy address space, spawn new process */
pid_t do_fork()
{
    /* Create new task */
    new_task = kmalloc(sizeof(struct task_t));
	new_task->regs = NULL;

    /* Clone address space */
    struct page_directory_t* dir = clone_page_dir(cur_dir);

    new_task->pid = next_pid++;
    new_task->page_dir = dir;
    new_task->parent = cur_task;
    new_task->next = NULL;
    new_task->stack = create_stack(STACK_SIZE);

    memcpy(new_task->stack->start, cur_task->stack->start, STACK_SIZE);

    /* Add to task queue */
    struct task_t* tmp = task_queue;
    while(tmp->next)
        tmp = tmp->next;
    tmp->next = new_task;

    /* Let scheduler fill the data to the task struct */
    __asm__ __volatile__("int $32");
    
    kprintf("Forking (pid: %u)\n", get_pid());
    if (cur_task->next != NULL)
    {
        cli();
        uint32_t esp;
        __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
        dump_stack_esp(cur_task->stack, esp);
        
        //for(;;);
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

