/* This file is a part of the RazOS project
 *
 * sys_execve.c -- execve() -syscall
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <string.h>
#include <kmalloc.h>
#include <util.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

#include "syscall.h"
#include "sys_execve.h"
#include "../loader/exec.h"
#include "../mm/paging.h"
#include "../mm/task.h"


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

/* Returns execution at 0x10000000 when succesful */
uint32_t sys_execve(struct registers_t* regs)
{
	/* Copy argv, envp to kernel memory */
	char** argv = (char**)REG_ARG2(regs);
	char** envp = (char**)REG_ARG3(regs);

	int argc = count(argv);
	int envc = count(envp);

	char** args = NULL;
	
	if (argc > 0)
	{
		args = (char**)kmalloc(argc * sizeof(char*));
		if (args == NULL)
			return -1;
		
		for(int i = 0; i < argc; i++)
		{
			args[i] = (char*)kmalloc(strlen(argv[i]) + 1);
			if (args[i] == NULL)
				return -1; /* kmalloc sets errno */
			
			strcpy(args[i], argv[i]);
			args[i][strlen(argv[i])] = '\0';
		}
	}

	char** env = NULL;
	
	if (envc > 0)
	{
		env = (char**)kmalloc(envc * sizeof(char*));
		if (env == NULL)
			return -1; /* kmalloc sets errno */

		for(int i = 0; i < envc; i++)
		{
			env[i] = (char*)kmalloc(strlen(argv[i]) + 1);
			if (env[i] == NULL)
				return -1; /* kmalloc sets errno */

			strcpy(env[i], envp[i]);
			env[i][strlen(envp[i])] = '\0';
		}
	}

	/* change the program image */
	char* path = (char*)REG_ARG1(regs);
	int ret = exec(path);
	if (ret < 0)
		return -1; /* exec sets errno */

	/* Close files that have O_CLOEXEC set */
	for (int i = 0; i < OPEN_MAX; i++)
	{
		if (cur_task->files[i].oflag & O_CLOEXEC)
			close(i);
	}

	/* Set up the stack (store envp, argv contents) */
	char* stack = (char*)USER_STACK_END;

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
	
	env = (char**)stack; /* env = address of envp on user stack */

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
	
	args = (char**)stack;
	
	/* Finally, store argc */
	stack = push(stack, (uint32_t)argc, sizeof(uint32_t));

	REG_ARG2(regs) = (uint32_t)args;
	REG_ARG3(regs) = (uint32_t)env;
	
	regs->ecx = (uint32_t)stack;
	regs->edx = USER_CODE_BEGIN;
	
	return (uint32_t)ret;
}
