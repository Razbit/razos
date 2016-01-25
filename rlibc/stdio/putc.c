/* This file is a part of the RazOS project
 *
 * putc.c -- dputc, a C99 extension; putchar
 *
 * Razbit 2016 */

#include <stdio.h>
#include <unistd.h>

int dputc(int chr, int fd)
{
	return write(fd, (char*)&chr, 1);
}

int putchar(int c)
{
	return write(STDOUT_FILENO, (char*)&c, 1);
}
