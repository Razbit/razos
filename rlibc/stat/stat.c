/* This file is a part of the RazOS project
 *
 * stat.c -- file status information.
 *
 * Razbit 2016 */

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
