/* This file is a part of the RazOS project
 *
 * exec.c -- do_execve(), the implementation behind the sys_execve()
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <kmalloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <console.h>

#include "../mm/task.h"
#include "../mm/paging.h"

#include "elf.h"


int do_execve(char* path, char** argv, char** envp)
{
	/* TODO: argv */
	/* TODO: envp */
	/* TODO: mangle syscall registers to set ecx, edx so that we
	 * return to the new program image's _start with an empty stack. */

	int fd = open(path, O_RDONLY);
	if (fd < 0)
		goto bad;
	
	size_t size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	uint8_t* buf = kmalloc(size);
	if (buf == NULL)
		goto bad;

	/* Clean up the user-space. Leaves stack as-is, so leave a todo */
	uint32_t page = USER_CODE_BEGIN;
	for (; page < USER_STACK_BEGIN; page += PAGE_SIZE)
	{
		if (page_mapped_to_user(page))
			page_unmap(page);
	}
	
	read(fd, buf, size);
	
	void* addr = load_elf(buf);
	if ((int)addr < 0)
		goto bad;

	/* User heap area begins at the next page table boundary */
	cur_task->uheap_begin = (uint32_t)(addr + 1024 * PAGE_SIZE)\
		& ~(1024 * PAGE_SIZE -1);
	cur_task->uheap_end = cur_task->uheap_begin;
	
	kfree(buf);
	close(fd);

	return 0;
	
bad:
	return -1;
}
