/* sys_fs.c -- filesystem-related syscalls */

/* Copyright (c) 2015, 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#include <sys/types.h>
#include <sys/stat.h>

#include <api/razos.h>
#include "syscall.h"

#include "../fs/vfs.h"

#include "sys_fs.h"


uint32_t sys_read(struct registers_t* regs)
{
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

uint32_t sys_fcntl(struct registers_t* regs)
{
	int fd = (int)REG_ARG1(regs);
	int cmd = (int)REG_ARG2(regs);
	int arg = (uint32_t)REG_ARG3(regs);

	return do_fcntl(fd, cmd, arg);
}

uint32_t sys_fstat(struct registers_t* regs)
{
	int fd = (int)REG_ARG1(regs);
	struct stat* buf = (struct stat*)REG_ARG2(regs);

	return fstat(fd, buf);
}
