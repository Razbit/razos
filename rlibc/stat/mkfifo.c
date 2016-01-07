/* This file is a part of the RazOS project
 *
 * mkfifo.c -- function mkfifo() from stat.h (POSIX) for creating
 * FIFO-special files
 *
 * Razbit 2016 */

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
