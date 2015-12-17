/* This file is a part of the RazOS project
 *
 * pagefault.c -- pagefault handling
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#include <sys/types.h>
#include <panic.h>
#include <console.h>

#include "task.h"
#include "paging.h"

/* Handle pagefaults. Called in ISR 14 in isr.s */
/* TODO: highly WIP */
void pagefault_handler(uint32_t addr, uint32_t error, uint32_t eip)
{
	/* Where did the page fault happen? */
	if (addr >= USER_STACK_BEGIN && addr <= USER_STACK_END)
	{
		/* Page fault happend in the user stack area.
		 * Increase stack size (not over USER_STACK_BEGIN) */
		if (cur_task->stack_begin > USER_STACK_BEGIN)
		{
			page_map(cur_task->stack_begin - 0x10, page_alloc(), \
			         PE_PRESENT | PE_RW | PE_USER);
			cur_task->stack_begin -= PAGE_SIZE;

			return;
		}
	}

general:
	panic("Page fault at 0x%p (0x%x) eip: 0x%p\n", addr, error, eip);
}
