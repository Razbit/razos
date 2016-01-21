/* This file is a part of the RazOS project
 *
 * task.c -- tasking and switching
 *
 * Razbit 2015, 2016 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <console.h>
#include <kassert.h>
#include <panic.h>
#include <util.h>
#include <string.h>
#include <kmalloc.h>
#include <fcntl.h>
#include <unistd.h>

#include "../gdt.h"
#include "kernel_page.h"
#include "paging.h"
#include "sched.h"
#include "task.h"

static struct tss_t tss;
struct task_t* cur_task;
static struct task_t* tasks[1024];


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

		tasks[pid] = kmalloc(sizeof(struct task_t));
		if (!tasks[pid])
			panic("Tasking: could not allocate space for task_t\n");
		tasks[pid]->state = TASK_STATE_READY;
		tasks[pid]->pid = pid;
		tasks[pid]->stack_begin = USER_STACK_END;
		tasks[pid]->wait_queue.live.head = NULL;
		tasks[pid]->wait_queue.live.tail = NULL;
		return tasks[pid];
	}

	return NULL;
}

static void copy_user_pages(struct task_t* new_task)
{
	new_task->uheap_begin = cur_task->uheap_begin;
	new_task->uheap_end = cur_task->uheap_end;
	
	uint32_t* cur_page_dir = (uint32_t*)CUR_PG_DIR;

	/* Copy available page tables */
	for (size_t dir_i = USER_MEMORY_BEGIN / (PAGE_SIZE * 1024); \
	     dir_i < 1023; dir_i++)
	{
		if (!cur_page_dir[dir_i])
			continue;

		uint32_t* cur_page_table = \
			(uint32_t*)(CUR_PG_TABLE_BASE + dir_i * PAGE_SIZE);
		uint32_t* new_page_table = kmalloc_pa(PAGE_SIZE);

		if (!new_page_table)
			panic("tasking: could not allocate page table for user\n");

		new_task->page_dir[dir_i] = virt_to_phys((uint32_t)new_page_table) \
			| (cur_page_dir[dir_i] & PE_FLAG_MASK);

		/* Copy available pages */
		for(size_t tab_i = 0; tab_i < 1024; tab_i++)
		{
			uint32_t cur_entry = cur_page_table[tab_i];
			void* cur_virt = \
				(void*)((dir_i*PAGE_SIZE*1024)+(tab_i*PAGE_SIZE));
			if (!(cur_entry & PE_PRESENT))
				continue;

			uint32_t new_page = page_alloc();
			void* new_page_mapping = page_temp_map(new_page);
			memcpy(new_page_mapping, cur_virt, PAGE_SIZE);
			page_temp_unmap();

			new_page_table[tab_i] = new_page | (cur_entry & PE_FLAG_MASK);
		}
	}
}

static void create_skel_page_dir(struct task_t* task)
{
	uint32_t* cur_page_dir = (uint32_t*)CUR_PG_DIR;
	uint32_t* task_page_dir = kmalloc_pa(PAGE_SIZE);

	if (!task_page_dir)
		panic("Tasking: could not allocate space for page directory\n");
	memset(task_page_dir, 0, PAGE_SIZE);
	
	task->kstack = kmalloc_pa(PAGE_SIZE);
	if (!task->kstack)
		panic("Tasking: could not allocate kstack\n");
	memset(task->kstack, 0, PAGE_SIZE);

	uint32_t* kstack_page_table = kmalloc_pa(PAGE_SIZE);
	if (!kstack_page_table)
		panic("Tasking: could not allocate kstack page table\n");
	memset(kstack_page_table, 0, PAGE_SIZE);
	
	task->page_dir = task_page_dir;
	task->page_dir_phys = virt_to_phys((uint32_t)task_page_dir);
	task->page_dir[1023] = task->page_dir_phys | PE_PRESENT | PE_RW;

	if (task->pid != 1)
		copy_user_pages(task);

	/* Map kernel code, heap and stack to same locations */
	for (size_t i = 0; i < KERNEL_STACK_BEGIN / (1024 * PAGE_SIZE); i++)
		task_page_dir[i] = cur_page_dir[i];

	task->page_dir[KERNEL_STACK_BEGIN / (1024 * PAGE_SIZE)] = \
		virt_to_phys((uint32_t)kstack_page_table) | PE_PRESENT | PE_RW;

	kstack_page_table[1023] = \
		virt_to_phys((uint32_t)(task->kstack)) | PE_PRESENT | PE_RW;

	for (int i = 0; i < 1023; i++)
		kstack_page_table[i] = page_alloc() | PE_PRESENT | PE_RW;
}

/* Initialize tasking */
void task_init()
{
	gdt_set_tss(GDT_TSS, (uint32_t)&tss, sizeof(tss));
	gdt_reload();

	tss.ss0 = GDT_KERNEL_DATA;
	tss.esp0 = KERNEL_STACK_END;

	/* Pointer to io perm bitmap is beyond the end of the segment */
	tss.iopb = sizeof(tss);

	kprintf("Load TSS at 0x%p\n", &tss);

	/* load tss selector */
	__asm__ __volatile__("mov %0, %%eax; ltr %%ax" \
	                     :: "r"(GDT_TSS | 3) : "%eax");

	cur_task = alloc_empty_task();
	create_skel_page_dir(cur_task);
	
	cur_task->ppid = 0;
		
	set_page_directory(cur_task->page_dir_phys);
	
	/* Create user stack */
	kprintf("Allocating user stack\n");
	page_map(USER_STACK_END, page_alloc(), PE_PRESENT | PE_USER | PE_RW);
	cur_task->stack_begin -= PAGE_SIZE;
	
	kprintf("Tasking initialization succeeded.\n");
}

void task_init_stdio()
{
	int stdin_fd = open("stdin", O_RDWR);
	int stdout_fd = open("stdout", O_WRONLY);
	int stderr_fd = open("stderr", O_WRONLY);

	kprintf("Standard io opened as %i %i %i\n", \
	        stdin_fd, stdout_fd, stderr_fd);
}

/* TODO: what if we run out of memory? Now we just panic or PF */
struct task_t* task_fork_inner()
{
	struct task_t* new_task = alloc_empty_task();	
	create_skel_page_dir(new_task);

	memcpy(&new_task->fpu_state, &cur_task->fpu_state, \
	       sizeof(new_task->fpu_state));

	memcpy(new_task->kstack, cur_task->kstack, PAGE_SIZE);

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
	tasks[task->pid] = NULL;

	kfree(task->kstack);

	/* TODO: Free pages allocated for the task in its page directory */

	kfree(task->page_dir);
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
