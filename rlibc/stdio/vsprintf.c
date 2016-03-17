/* This file is a part of the RazOS project
 *
 * vsprintf.c -- vsprintf()
 *
 * Razbit 2016 */

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
