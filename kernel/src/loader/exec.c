/* This file is a part of the RazOS project
 *
 * exec.c -- execve(), the implementation behind the sys_exec()
 *
 * Razbit 2015, 2016 */

#include <sys/types.h>
#include <kmalloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <console.h>
#include <string.h>
#include <util.h>
#include <limits.h>
#include <errno.h>

#include "../mm/task.h"
#include "../mm/paging.h"

#include "elf.h"


/* Count the argument strings */
static int count(char** v)
{
	if (v == NULL)
		return 0;

	int i = 0;

	while (v[i] != NULL)
		i++;

	return i;
}

/* "push" data to the new user stack */
static char* push(char* sp, uint32_t data, size_t size)
{
	switch (size)
	{
	case sizeof(char):
		sp--;
		*sp = (char)data;
		break;
	case sizeof(char*):
		sp = push(sp, (data>>24)&0xff, sizeof(char));
		sp = push(sp, (data>>16)&0xff, sizeof(char));
		sp = push(sp, (data>>8)&0xff, sizeof(char));
		sp = push(sp, data&0xff, sizeof(char));
		break;
	}
	return sp;
}

uint32_t* execve(char* path, char** argv, char** envp)
{
	int argc = count(argv);
	int envc = count(envp);

	static uint32_t ret[3];
	char** args = NULL;

	if (argc > 0)
	{
		args = (char**)kmalloc(argc * sizeof(char*));
		if (args == NULL)
			return NULL;

		for(int i = 0; i < argc; i++)
		{
			args[i] = (char*)kmalloc(strlen(argv[i]) + 1);
			if (args[i] == NULL)
				return NULL; /* kmalloc sets errno */

			strcpy(args[i], argv[i]);
			args[i][strlen(argv[i])] = '\0';
		}
	}

	char** env = NULL;

	if (envc > 0)
	{
		env = (char**)kmalloc(envc * sizeof(char*));
		if (env == NULL)
			return NULL; /* kmalloc sets errno */

		for(int i = 0; i < envc; i++)
		{
			env[i] = (char*)kmalloc(strlen(argv[i]) + 1);
			if (env[i] == NULL)
				return NULL; /* kmalloc sets errno */

			strcpy(env[i], envp[i]);
			env[i][strlen(envp[i])] = '\0';
		}
	}

	/* Open the program image */
	/* TODO: check exec permission, possibly fail with EACCESS.
	 * Will be implemented when we have meaningful permissions */
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		errno = ENOENT;
		return NULL;
	}

	size_t size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	uint8_t* buf = kmalloc(size);
	if (buf == NULL)
	{
		errno = ENOMEM;
		return NULL;
	}

	/* TODO: We should check the executable before destroying
	 * the user-space.. */

	/* Clean up the user-space */
	uint32_t page = UMEM_BEGIN;
	/* Latter check because of integer overflow */
	while (page < USTACK_END && page >= UMEM_BEGIN)
	{
		uint32_t flags = page_flags(page, cur_task->page_dir);	
		if (!((flags >> 16) & PF_PRES))
			page += 1024 * PAGE_SIZE;

		if ((flags & (PF_PRES | PF_USER)) == (PF_PRES | PF_USER))
		{
			frame_free(get_phys(page, cur_task->page_dir));
			page_map(page, 0, 0, cur_task->page_dir);
		}
		page += PAGE_SIZE;
	}

	/* Create user stack */
	if (page_map(USTACK_END, frame_alloc(), \
	             PF_PRES | PF_USER | PF_RW, cur_task->page_dir) == NULL)
	{
		/* page_map sets errno */
		kfree(buf);
		close(fd);
		return NULL;
	}
	if (page_map(USTACK_END - PAGE_SIZE, frame_alloc(), \
	             PF_PRES | PF_USER | PF_RW, cur_task->page_dir) == NULL)
	{
		/* page_map sets errno */
		kfree(buf);
		close(fd);
		return NULL;
	}

	if (read(fd, buf, size) != (ssize_t)size)
	{
		kfree(buf);
		close(fd);
		return NULL;
	}

	void* addr = load_elf(buf);
	if ((int)addr < 0)
	{
		/* load_elf sets errno */
		kfree(buf);
		close(fd);
		return NULL;
	}

	/* User heap area begins at the next page table boundary */
	cur_task->uheap_begin = \
		(void*)((uint32_t)(addr+1024*PAGE_SIZE) & ~(1024*PAGE_SIZE-1));
	cur_task->uheap_end = cur_task->uheap_begin;

	kfree(buf);
	close(fd);

	/* Close files that have O_CLOEXEC set */
	for (int i = 0; i < OPEN_MAX; i++)
	{
		if (cur_task->files[i].oflag & O_CLOEXEC)
			close(i);
	}

	/* Set up the stack (store envp, argv contents) */
	char* stack = (char*)USTACK_END;

	/* Copy the strings, store pointers to them */
	if (envc > 0)
	{
		for (int i = envc - 1; i >= 0; i--)
		{
			stack = push(stack, (uint32_t)'\0', sizeof(char));
			for (int j = strlen(env[i])-1; j >= 0; j--)
				stack = push(stack, (uint32_t)env[i][j], sizeof(char));

			kfree(env[i]);
			env[i] = stack; /* Store string's new address to env */
		}

		/* env now contains pointers to the env strings on user stack */
		stack = (char*)round_down((uint32_t)stack, sizeof(char*));
		stack = push(stack, 0, sizeof(char*)); /* envp 0-terminated */
		for (int i = envc-1; i >= 0; i--)
			stack = push(stack, (uint32_t)env[i], sizeof(char*));
		kfree(env);
	}
	else
	{
		stack = push(stack, 0, sizeof(char*));
	}

	ret[0] = (uint32_t)stack; /* address of envp on user stack */

	if (argc > 0)
	{
		for (int i = argc - 1; i >= 0; i-- )
		{
			stack = push(stack, (uint32_t)'\0', sizeof(char));
			for (int j = strlen(args[i])-1; j >= 0; j--)
				stack = push(stack, (uint32_t)args[i][j], sizeof(char));

			kfree(args[i]);
			args[i] = stack;
		}

		stack = (char*)round_down((uint32_t)stack, sizeof(char*));
		stack = push(stack, 0, sizeof(char*)); /* 0-terminated */
		for (int i = argc-1; i >= 0; i--)
			stack = push(stack, (uint32_t)args[i], sizeof(char*));
		kfree(args);
	}
	else
	{
		stack = push(stack, 0, sizeof(char*));
	}

	ret[1] = (uint32_t)stack; /* Address of argv on user stack */

	/* Finally, store argc */
	ret[2] = (uint32_t)push(stack, (uint32_t)argc, sizeof(uint32_t));

	return ret;
}
