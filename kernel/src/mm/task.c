/* This file is a part of the RazOS project
 *
 * task.c -- tasking and switching
 *
 * Razbit 2015, 2016 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <asm/system.h>
#include <console.h>
#include <kassert.h>
#include <panic.h>
#include <util.h>
#include <string.h>
#include <kmalloc.h>
#include <fcntl.h>
#include <unistd.h>

#include "../gdt.h"
#include "paging.h"
#include "sched.h"
#include "task.h"

struct task_t* cur_task = NULL;

static struct tss_t tss = {0};
static struct task_t* tasks[1024] = {NULL};

struct task_t* get_task(pid_t pid)
{
	if (pid > countof(tasks))
		return NULL;
	return tasks[pid];
}

static struct task_t* alloc_empty_task()
{
	for(pid_t pid = 1; pid < countof(tasks); pid++)
	{
		/* PID taken */
		if (tasks[pid])
			continue;

		tasks[pid] = (struct task_t*)kmalloc(sizeof(struct task_t));
		if (!tasks[pid])
			panic("Tasking: could not allocate space for task_t\n");

		tasks[pid]->state = TASK_STATE_READY;
		tasks[pid]->pid = pid;
		tasks[pid]->wait_queue.live.head = NULL;
		tasks[pid]->wait_queue.live.tail = NULL;
		return tasks[pid];
	}

	return NULL;
}

/* Initialize tasking */
void task_init()
{
	tss.ss0 = GDT_KERNEL_DATA;
	tss.esp0 = KSTACK_END; /* We use this when handling interrupts */
	
	/* Pointer to io permission bitmap beyond the end of the segment */
	tss.iopb = sizeof(tss);
	
	gdt_set_tss(GDT_TSS, (uint32_t)&tss, sizeof(tss));
	gdt_reload();

	kprintf("Load TSS at 0x%p\n", &tss);

	/* load tss selector */
	__asm__ __volatile__("mov %0, %%eax; ltr %%ax" \
	                     :: "r"(GDT_TSS | 3) : "%eax");

	cur_task = alloc_empty_task();
	cur_task->page_dir = cur_page_dir;
	cur_task->ppid = 0;

	kprintf("Tasking initialization succeeded.\n");
}

/* TODO: what if we run out of memory? Now we just panic or PF */
struct task_t* task_fork_inner()
{

	struct task_t* new_task = alloc_empty_task();
	kprintf("Fork: allocated new task %u at 0x%p\n", new_task->pid, new_task);
//	for(;;);
	new_task->page_dir = create_page_dir();
	clone_page_dir(new_task->page_dir, cur_task->page_dir);

	memcpy(&new_task->fpu_state, &cur_task->fpu_state, \
	       sizeof(new_task->fpu_state));

	memcpy(&new_task->files, &cur_task->files, \
	       sizeof(new_task->files));

	new_task->ppid = cur_task->pid;
	new_task->syscall_regs = cur_task->syscall_regs;

	/* vm cloned -> errno loc doesn't change */
	new_task->errno_loc = cur_task->errno_loc; 

	return new_task;
}

void task_kill(struct task_t* task, uint32_t status)
{
	task->exit_status = status;

	struct task_t* parent = get_task(task->ppid);

	/* If there are children in this task's wait queue we clean them up.
	 * Nothing really gives a single sh1t about them anymore.. */
	struct task_t* waitable_child = task->wait_queue.live.head;
	while(waitable_child)
	{
		struct task_t* next_waitable_child = \
			waitable_child->wait_queue.dead.next;
		task_destroy(waitable_child);
		waitable_child = next_waitable_child;
	}

	/* Reparent children */
	for (pid_t i = 2; i < countof(tasks); i++)
	{
		struct task_t* child = get_task(i);
		if (child && child->ppid == task->pid)
			child->ppid = 1;
	}

	/* Insert this task into the parent's wait queue */
	if (parent->wait_queue.live.tail)
	{
		parent->wait_queue.live.tail->wait_queue.dead.next = task;
		parent->wait_queue.live.tail = task;
	}
	else
	{
		parent->wait_queue.live.head = task;
		parent->wait_queue.live.tail = task;
	}

	task->wait_queue.dead.next = NULL;

	/* Wake parent if it is blocked in wait() */
	if (parent->state == TASK_STATE_BLOCK_WAIT)
		parent->state = TASK_STATE_READY;

	/* Scheduler should no schedule this task anymore */
	task->state = TASK_STATE_EXITING;
}

void task_destroy(struct task_t* task)
{
	/* Free user space */
	for (size_t i = SC_STACK_BEGIN; i < USTACK_END; i += PAGE_SIZE)
	{
		/* Check if page is present */
		uint32_t flags = page_flags(i, task->page_dir);
		if ((flags & PF_PRES) == 0)
			continue;

		frame_free(get_phys(i, task->page_dir));
		page_map(i, 0, 0, task->page_dir);
	}

	clear_page_dir(task->page_dir);
	kfree(task->page_dir);

	tasks[task->pid] = NULL;
	kfree(task);
}

struct task_t* sched_next()
{
	pid_t cur_pid = cur_task->pid;

	for (size_t i = cur_pid + 1; i < countof(tasks); i++)
	{		
		struct task_t* task = get_task(i);

		if (!task)
			continue;
		if (task->state == TASK_STATE_READY)
			return task;
	}

	for (size_t i = 1; i <= cur_pid; i++)
	{		
		struct task_t* task = get_task(i);

		if (!task)
			continue;
		if (task->state == TASK_STATE_READY)
			return task;
	}
	
	panic("No tasks ready to schedule!");
}

/* Used in isrs.s when calling scheduler */
int sched_halted = 0;

void sched_halt()
{
	sched_halted = 1;
}

void sched_cont()
{
	sched_halted = 0;
}
