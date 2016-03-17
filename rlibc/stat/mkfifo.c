/* mkfifo.c -- function mkfifo() from stat.h (POSIX) for creating
 * FIFO-special files */

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
#include <fcntl.h>
#include <unistd.h>

int mkfifo(const char* path, mode_t mode)
{
	int fd = creat(path, mode | S_IFIFO);
	if (fd < 0)
		return -1;
	close(fd);
	return 0;
}
