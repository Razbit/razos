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

#include "task.h"

/* Currently running task */
volatile struct task_t* cur_task = 0;
/* The task list */
volatile struct task_t* task_queue = 0;

/* In paging.c */
extern struct page_directory_t *kernel_dir;
extern struct page_directory_t *cur_dir;

/* In process.s */
extern uint32_t read_eip();

/* Counter for PIDs */
uint32_t next_pid = 1;

/* Initialize the tasking system */
void init_tasking()
{
    kprintf("Initializing tasking\n");
    /* Initialize the first task (kernel) */
    cur_task = task_queue = kmalloc(sizeof(struct task_t));
    cur_task->pid = next_pid++;
    cur_task->esp = 0;
    cur_task->ebp = 0;
    cur_task->eip = 0;
    cur_task->page_dir = cur_dir;
    cur_task->next = NULL;
}

/* Called in interrupt/timer.c */
void schedule()
{
    /* If tasking is not yet initialized, just return */
    if (!cur_task)
        return;
    
    /* Read esp, ebp, eip */
    uint32_t esp, ebp, eip;
    __asm__ __volatile__ ("mov %%esp, %0" : "=r"(esp));
    __asm__ __volatile__ ("mov %%ebp, %0" : "=r"(ebp));

    /* When reading the instruction pointer, we might be in either
     * of two states:
     * 1) We called the function and it returned the EIP
     * 2) We just switched tasks and thus came back here since we
     *    start execution from this EIP -> return immediately */
    eip = read_eip();    
    //kprintf("EIP: 0x%x N: %u\n", eip);
    
    if (eip == 0xB16B00B5) /* Magic boobies for figuring out the above */
    {
        kprintf("BOOBIEEESSS!!!!1!!1!!\n");
        //panic("Sched");
        return;
    }
    else
        kprintf("No boobies :(   (EIP: 0x%x)\n", eip);
    
    /* We did not switch tasks yet, so let us continue */
    /* Save the task state */
    cur_task->eip = eip;
    cur_task->esp = esp;
    cur_task->ebp = ebp;

    /* Get the next task */
    cur_task = cur_task->next;
    /* Start from the beginning of the list if we are at the end */
    if (!cur_task)
        cur_task = task_queue;

    eip = cur_task->eip;
    esp = cur_task->esp;
    ebp = cur_task->ebp;
    
    cur_dir = cur_task->page_dir;
    kprintf("EIP: 0x%x ESP: 0x%x EBP: 0x%x DIR: 0x%x\n", eip, esp, ebp, cur_dir);
    
    cli();
    __asm__ __volatile__("mov %0, %%ecx" :: "r"(eip));
    __asm__ __volatile__("mov %0, %%esp" :: "r"(esp));
    __asm__ __volatile__("mov %0, %%ebp" :: "r"(ebp));
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(cur_dir->physaddr));
    __asm__ __volatile__("mov $0xB16B00B5, %eax");
    sti();

    __asm__ __volatile__("jmp %ecx");
}

/* UNIX fork(): copy address space, spawn new process */
pid_t do_fork()
{
    cli();

    /* Current task is the parent of the new one */
    struct task_t* parent_task = cur_task;

    /* Clone address space */
    struct page_directory_t* dir = clone_page_dir(cur_dir);

    /* Create a new process */
    struct task_t* new_task = kmalloc(sizeof(struct task_t));

    new_task->pid = next_pid++;
    new_task->esp = 0;
    new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_dir = dir;
    new_task->next = 0;

    /* Add to the task_queue */
    struct task_t* tmp = task_queue;
    while (tmp->next)
        tmp = tmp->next;
    tmp->next = new_task;

    /* Here we will enter the new process */
    uint32_t eip = read_eip();

    /* We can now be either the parent or the child (after scheduling)*/
    if (cur_task = parent_task)
    {
        /* We are the parent -> set up the child */
        uint32_t esp, ebp;
        __asm__ __volatile__ ("mov %%esp, %0" : "=r"(esp));
        __asm__ __volatile__ ("mov %%ebp, %0" : "=r"(ebp));
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;

        sti();

        return new_task->pid;        
    }
    else
    {
        //sti();
        /* We are the child -- return 0 */
        return 0;
    }
}

/* Return PID of the current process */
pid_t get_pid()
{
    return cur_task->pid;
}
