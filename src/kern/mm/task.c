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

    /* Use the scheduler to finalize */
    __asm__ __volatile__("int $32");
}

/* Called in interrupt/timer.c */
void schedule(struct register_t* regs)
{
    /* Return if tasking is not yet initialized */
    if (!cur_task)
        return;

    /* Copy current context to the task struct */
    memcpy(cur_task->regs, regs, sizeof(struct register_t));
        
    /* Get next task in queue */
    cur_task = cur_task->next;
    if (!cur_task)
        cur_task = task_queue;
    
    /* This is the last-forked task (forking incomplete)
     * -> finalize forking */
    if (cur_task->regs == NULL)
    {
        kprintf("Finalizing forking: pid %u\n", get_pid());
        
        cur_task->regs = kmalloc(sizeof(struct register_t));
        memcpy(cur_task->regs, cur_task->parent->regs,  \
               sizeof(struct register_t));
        
    }
    else
    {
        switch_page_dir(cur_task->page_dir);
        uint32_t stack_end =                                            \
            (uint32_t)cur_task->stack->start + cur_task->stack->size;
        cur_task->stack->offset = stack_end - cur_task->regs->esp;
        
        /* Setup execution context */
        memcpy(regs, cur_task->regs, sizeof(struct register_t));
    }
}

struct task_t* fork_inner()
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
    /* TODO:
     * This actually has to be mapped to the same virtual address as
     * the original (but is still memcpy'd). Change name to kstack.
     * Need 2 stacks, kstack created here and user stack in exec()
     */
    new_task->stack = create_stack(STACK_SIZE);

    /* Add to task queue */
    struct task_t* tmp = task_queue;
    while(tmp->next)
        tmp = tmp->next;
    tmp->next = new_task;

    /* Copy the stack */
    memcpy(new_task->stack->start, cur_task->stack->start, STACK_SIZE);
    
    /* Let scheduler fill the data to the task struct */
    __asm__ __volatile__("int $32");

    return new_task;
}

/* Return PID of the current process */
pid_t get_pid()
{
    return cur_task->pid;
}

