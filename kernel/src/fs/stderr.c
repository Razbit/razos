/* This file is a part of the RazOS project
 *
 * stderr.c -- standard error output
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <console.h> /* For now, a better terminal on the way */

#include "../mm/task.h"

#include "vfs.h"
#include "ramfs.h"
#include "stdio_vfs.h"

static ssize_t write_stderr(int fd, const void* buf, size_t size);
static int open_stderr(struct vfs_node_t* node, int oflag);
	
/* Initialize stderr. We use modified ramfs nodes in the background */
int init_stderr()
{
	int fd = open_vfs("stderr", O_RDWR | O_CREAT);

	/* Now that we have a file in ramfs, let's modify the vfs_node */
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	node->write = &write_stderr;
	node->read = NULL;
	node->open = &open_stderr;

	close_vfs(fd);
	
	return 1;
}

static ssize_t write_stderr(int fd, const void* buf, size_t size)
{
	size_t i = 0;
	for (; i < size; i++)
	{
		kputchar(((char*)buf)[i]);
	}
}

/* Only used for when opening as fd 2 */
static int open_stderr(struct vfs_node_t* node, int oflag)
{
	/* If fildes 2 isn't free anymore, we use the vfs-provided open() */
	if (cur_task->files[STDERR_FILENO].vfs_node != NULL)
		return -1;

	cur_task->files[STDERR_FILENO].vfs_node = node;
	cur_task->files[STDERR_FILENO].at = 0;
	cur_task->files[STDERR_FILENO].oflag = oflag;

	return STDERR_FILENO;
}