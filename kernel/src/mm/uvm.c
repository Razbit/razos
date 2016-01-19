/* This file is a part of the RazOS project
 *
 * uvm.c -- user virtual memory (mainly heap area). Designed
 * to be simple, is heavily based on brk/sbrk
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <util.h>

#include "paging.h"
#include "task.h"


/* Allocate the first available, sequential page for uvm use,
 * return its address */
void* user_page_alloc()
{
	/* Make sure there is virtual address space left to allocate */
	if (cur_task->uheap_end < (void*)USER_STACK_BEGIN)
	{
		page_map((uint32_t)(cur_task->uheap_end), page_alloc(), \
		         PE_USER | PE_RW | PE_PRESENT);
		void* ret = cur_task->uheap_end;
		cur_task->uheap_end += PAGE_SIZE;
		return ret;
	}
	else
	{
		return NULL;
	}
}

/* Free the last page  allocated to uvm */
void user_page_free()
{
	void* addr = cur_task->uheap_end - PAGE_SIZE;
	if (page_mapped_to_user((uint32_t)addr))
		page_unmap((uint32_t)addr);
	cur_task->uheap_end -= PAGE_SIZE;
}

/* POSIX brk(). Set end of heap to addr */
int uvm_brk(void* addr)
{
	addr = (void*)round_up((uint32_t)addr, PAGE_SIZE);
	if (addr < (void*)USER_MEMORY_BEGIN)
		goto bad;
	if (addr >= (void*)USER_STACK_BEGIN)
		goto bad;
	
	/* cur_task->uheap_next_free tells us the size of the heap */
	if (addr == cur_task->uheap_end)
	{
		return 0;
	}
	else if (addr > cur_task->uheap_end)
	{
		/* Enlarge the heap */
		while(addr > cur_task->uheap_end)
		{
			if (user_page_alloc() == NULL)
				goto bad;
		}
		return 0;
	}
	else
	{
		/* Shrink the heap */
		while(addr < cur_task->uheap_end)
			user_page_free();
		return 0;
	}

bad:
	/* TODO: errno = ENOMEM */
	return -1;
}

/* POSIX sbrk() */
void* uvm_sbrk(intptr_t incr)
{
	void* ret = cur_task->uheap_end;
		
	if (incr > 0)
	{
		/* Enlarge heap by incr bytes */
		while (incr > 0)
		{
			if (user_page_alloc() == NULL)
				goto bad;
			incr -= PAGE_SIZE;
		}
	}
	else if (incr < 0)
	{
		/* Shrink the heap */
		if (cur_task->uheap_end + incr < cur_task->uheap_begin)
			goto bad;
		
		incr += PAGE_SIZE-1;
		while (incr < 0)
		{
			user_page_free();
			incr += PAGE_SIZE;
		}
	}
	return ret;

bad:
	/* TODO: errno = ENOMEM */
	return (void*)-1;
}
