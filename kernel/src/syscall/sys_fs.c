/* This file is a part of the RazOS project
 *
 * sys_fs.c -- filesystem-related syscalls
 *
 * Razbit 2015 */

#include <sys/types.h>

#include <api/razos.h>
#include "syscall.h"

#include "../fs/vfs.h"

#include "sys_fs.h"


uint32_t sys_read(struct registers_t* regs)
{
	if (!valid_user_buffer((uint32_t)REG_ARG2(regs), \
	                       (size_t)REG_ARG3(regs)))
	{
		return (uint32_t)-1;
	}
	
	int fd = (int)REG_ARG1(regs);
	void* buf = (void*)REG_ARG2(regs);
	size_t size = (size_t)REG_ARG3(regs);

	return read_vfs(fd, buf, size);
}

uint32_t sys_write(struct registers_t* regs)
{
	int fd = (int)REG_ARG1(regs);
	const void* buf = (void*)REG_ARG2(regs);
	size_t size = (size_t)REG_ARG3(regs);

	if (buf == NULL)
		return (uint32_t)-1;
	else
		return write_vfs(fd, buf, size);
}

uint32_t sys_open(struct registers_t* regs)
{
	const char* name = (const char*)REG_ARG1(regs);
	int oflag = (int)REG_ARG2(regs);
	mode_t mode = (mode_t)REG_ARG3(regs);

	return open_vfs(name, oflag, mode);
}

uint32_t sys_close(struct registers_t* regs)
{
	int fd = (int)REG_ARG1(regs);
	return close_vfs(fd);
}

uint32_t sys_creat(struct registers_t* regs)
{
	const char* name = (const char*)REG_ARG1(regs);
	mode_t mode = (mode_t)REG_ARG2(regs);

	return creat_vfs(name, mode);
}

uint32_t sys_lseek(struct registers_t* regs)
{
	int fd = (int)REG_ARG1(regs);
	off_t offset = (off_t)REG_ARG2(regs);
	int whence = (int)REG_ARG3(regs);

	return lseek_vfs(fd, offset, whence);
}
