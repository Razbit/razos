/* This file is a part of the RazOS project
 *
 * uvm.c -- user virtual memory (mainly heap area)
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <errno.h>
#include <util.h>
#include <string.h>

#include "paging.h"
#include "task.h"

/* Allocate a page for uvm use, return its address */
void* uvm_page_alloc()
{
	/* Make sure there is virtual address space left to allocate */
	if (cur_task->uheap_end < (void*)USTACK_BEGIN)
	{
		void* ret = page_map((uint32_t)(cur_task->uheap_end), \
		                     frame_alloc(), PF_PRES | PF_RW | PF_USER,\
		                     cur_task->page_dir);
		if (ret == NULL)
			return NULL; /* page_map sets errno */

		cur_task->uheap_end += PAGE_SIZE;
		memset(ret, 0, PAGE_SIZE);
		return ret;
	}
	else
	{
		errno = ENOMEM;
		return NULL;
	}
}

/* Free the last page allocated to uvm */
void uvm_page_free()
{
	uint32_t addr = (uint32_t)cur_task->uheap_end - PAGE_SIZE;
	frame_free(get_phys(addr, cur_task->page_dir));
	page_map(addr, 0, 0, cur_task->page_dir);
	cur_task->uheap_end -= PAGE_SIZE;
}

/* POSIX brk(). Set end of heap to addr */
int uvm_brk(void* addr)
{
	addr = (void*)round_up((uint32_t)addr, PAGE_SIZE);
	if (addr < (void*)UMEM_BEGIN)
		goto bad;
	if (addr >= (void*)USTACK_BEGIN)
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
			if (uvm_page_alloc() == NULL)
				goto bad;
		}
		return 0;
	}
	else
	{
		/* Shrink the heap */
		while(addr < cur_task->uheap_end)
			uvm_page_free();
		return 0;
	}

bad:
	errno = ENOMEM;
	return -1;
}

/* POSIX sbrk() */
void* uvm_sbrk(intptr_t incr)
{
	void* ret = cur_task->uheap_end;

	if (incr > 0)
	{
		/* Enlarge heap by incr bytes */
		if (cur_task->uheap_end + incr > (void*)USTACK_BEGIN)
			goto bad;

		while (incr > 0)
		{
			if (uvm_page_alloc() == NULL)
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
			uvm_page_free();
			incr += PAGE_SIZE;
		}
	}

	return ret;

bad:
	errno = ENOMEM;
	return (void*)-1;
}
