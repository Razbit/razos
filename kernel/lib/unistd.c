/* unistd.h -- POSIX standard constants and types */

/* Copyright (c) 2015 Eetu "Razbit" Pesonen
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

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include "../src/fs/vfs.h"

off_t lseek(int fd, off_t offset, int whence)
{
	return lseek_vfs(fd, offset, whence);
}

ssize_t read(int fd, void* buf, size_t size)
{
	return read_vfs(fd, buf, size);
}

ssize_t write(int fd, const void* buf, size_t size)
{
	return write_vfs(fd, buf, size);
}

int dup(int fd)
{
	return fcntl(fd, F_DUPFD, 0);
}

int dup2(int fd, int fd2)
{
	close(fd2);
	return fcntl(fd, F_DUPFD, fd2);
}
