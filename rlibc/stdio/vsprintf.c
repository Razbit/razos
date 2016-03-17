/* vsprintf.c -- vsprintf() */

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

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

int vsprintf(char* buf, const char* fmt, va_list arg)
{
	int fd = open("__vsp_tmp", O_RDWR | O_CREAT, S_IFREG);
	if (fd < 0)
		return -1;

	lseek(fd, 0, SEEK_SET);

	int size = vdprintf(fd, fmt, arg);
	lseek(fd, 0, SEEK_SET);
	int ret = read(fd, buf, size);
	close(fd);

	return ret;
}
