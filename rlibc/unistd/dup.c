/* This file is a part of the RazOS project
 *
 * dup.c -- dup, dup2 from unistd (POSIX)
 *
 * Razbit 2016 */

#include <unistd.h>
#include <fcntl.h>

int dup(int fd)
{
	return fcntl(fd, F_DUPFD, 0);
}

int dup2(int fd, int fd2)
{
	close(fd2);
	return fcntl(fd, F_DUPFD, fd2);
}
