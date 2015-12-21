/* This file is a part of the RazOS project
 *
 * stdout.c -- standard output
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

static ssize_t write_stdout(int fd, const void* buf, size_t size);
static int open_stdout(struct vfs_node_t* node, int oflag);

/* Initialize stdout. We use modified ramfs nodes in the background */
int init_stdout()
{
	int fd = open_vfs("stdout", O_RDWR | O_CREAT);

	/* Now that we have a file in ramfs, let's modify the vfs_node */
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	node->write = &write_stdout;
	node->read = NULL;
	node->open = &open_stdout;

	close_vfs(fd);

	return 1;
}

static ssize_t write_stdout(int fd, const void* buf, size_t size)
{
	(void)fd;
	
	size_t i = 0;
	for (; i < size; i++)
	{
		kputchar(((char*)buf)[i]);
	}
}

/* Only used for when opening as fd 1 */
static int open_stdout(struct vfs_node_t* node, int oflag)
{
	/* If fildes 1 isn't free anymore, we use the vfs-provided open() */
	if (cur_task->files[STDOUT_FILENO].vfs_node != NULL)
		return -1;

	cur_task->files[STDOUT_FILENO].vfs_node = node;
	cur_task->files[STDOUT_FILENO].at = 0;
	cur_task->files[STDOUT_FILENO].oflag = oflag;

	return STDOUT_FILENO;
}
