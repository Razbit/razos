/* This file is a part of the RazOS project
 *
 * fcntl.c -- File control
 *
 * Razbit 2016 */

#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>

#include "vfs.h"
#include "../mm/task.h"


int do_fcntl(int fd, int cmd, uint32_t arg)
{
	/* If fd is not open */
	if (fd < 0 || fd >= OPEN_MAX || cur_task->files[fd].vfs_node == NULL)
	{
		errno = EBADF;
		return -1;
	}

	switch(cmd)
	{
	case F_DUPFD: /* Duplicate fd to lowest available after arg */
		if ((int)arg < 0)
		{
			errno = EINVAL;
			return -1;
		}
		int fd2 = get_free_fd((int)arg);
		if (fd2 < 0)
		{
			errno = EMFILE;
			return -1;
		}
		
		cur_task->files[fd2] = cur_task->files[fd];
		cur_task->files[fd2].oflag &= ~FD_CLOEXEC;

		return fd2;
		
	case F_GETFD: /* Get fd flags */
		return cur_task->files[fd].oflag & FD_MASK;
		
	case F_SETFD: /* Set fd flags */
		cur_task->files[fd].oflag &= ~FD_MASK; /* Clear all fd flags */
		cur_task->files[fd].oflag |= ((int)arg & FD_MASK);
		return 0;
		
	case F_GETFL: /* Get status flags */
		return cur_task->files[fd].oflag & O_MASK;
		
	case F_SETFL: /* Set status flags (ignore access and creation) */
		cur_task->files[fd].oflag &= ~O_SF_MASK; /* Clear status flags */
		cur_task->files[fd].oflag |= ((int)arg & O_SF_MASK);
		return 0;
		
	default:
		errno = EINVAL;
		return -1;
	}
}
