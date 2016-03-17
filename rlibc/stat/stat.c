/* stat.c -- file status information */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
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

#include <sys/stat.h>
#include <api/razos.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int stat(const char* path, struct stat* buf)
{
	if (path == NULL)
	{
		errno = ENOENT;
		return -1;
	}

	int fd = open(path, O_RDONLY);

	if (fd < 0)
	{
		errno = EBADF;
		return -1;
	}

	int ret = fstat(fd, buf);

	close(fd);

	return ret;
}

int fstat(int fd, struct stat* buf)
{
	return __syscall2(SYS_FSTAT, (uint32_t)fd, (uint32_t)buf);
}
