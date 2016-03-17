/* fcntl.c -- File control options */

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

#include <fcntl.h>
#include <sys/types.h>
#include <stdarg.h>
#include "../src/fs/vfs.h"

int open(const char* name, int oflag, ...)
{
	mode_t mode = 0;
	va_list args;
	va_start(args, oflag);
	if (oflag & O_CREAT)
		mode = va_arg(args, mode_t);
	va_end(args);

	return open_vfs(name, oflag, mode);
}

int creat(const char* name, mode_t mode)
{
	return creat_vfs(name, mode);
}

int close(int fd)
{
	return close_vfs(fd);
}

int fcntl(int fd, int cmd, ...)
{
	uint32_t arg;
	va_list args;
	va_start(args, cmd);

	switch(cmd)
	{
	case F_DUPFD:
	case F_SETFD:
	case F_SETFL:
		arg = va_arg(args, uint32_t);
		break;
	default:
		arg = 0;
		break;
	}

	va_end(args);

	return do_fcntl(fd, cmd, arg);
}
