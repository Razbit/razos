/* This file is a part of the RazOS project
 *
 * stat.c -- file status information. 
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include "vfs.h"
#include "../mm/task.h"

int stat(const char* path, struct stat* buf)
{
	if (path == NULL)
	{
		/* TODO: errno = ENOENT */
		return -1;
	}
	
	int fd = open(path, O_RDONLY);
	
	if (fd < 0)
	{
		/* TODO: errno = EBADF */
		return -1;
	}
	
	int ret = fstat(fd, buf);
	
	close(fd);

	return ret;
}

int fstat(int fd, struct stat* buf)
{
	if (fd < 0 || fd >= OPEN_MAX)
	{
		/* TODO: errno = EBADF */
		return -1;
	}

	*buf = cur_task->files[fd].vfs_node->status;
	
	return 0;
}
