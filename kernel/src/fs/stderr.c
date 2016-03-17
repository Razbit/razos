/* This file is a part of the RazOS project
 *
 * stderr.c -- standard error output
 *
 * Razbit 2015, 2016 */

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <console.h> /* For now, a better terminal on the way */
#include <string.h>
#include <kmalloc.h>
#include <errno.h>

#include "../mm/task.h"

#include "vfs.h"
#include "ramfs.h"
#include "stdio_vfs.h"

static ssize_t write_stderr(int fd, const void* buf, size_t size);
static int open_stderr(struct vfs_node_t* node, int oflag, mode_t mode);
	
/* Initialize stderr. We use modified ramfs nodes in the background */
int init_stderr()
{
	/* stderr already open? */
	if (cur_task->files[STDERR_FILENO].vfs_node != NULL)
		return STDERR_FILENO;

    struct vfs_node_t* node = vfs_root;

	/* If this is the first file */
	if (node == NULL)
	{
		node = (struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		if (node == NULL)
			return -1;

		vfs_root = node;
	}
	else
	{
		/* Find the end of vfs */
		while (node->next != NULL)
			node = node->next;

		node->next = \
			(struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		if (node->next == NULL)
			return -1;

		node = node->next;
	}

	node->status.st_mode = 0;
	node->status.st_uid = 0;
	node->status.st_gid = 0;
	node->status.st_size = 0;
	node->next = NULL;
	strcpy(node->name, "stderr");

	node->status.st_dev = DEVID_STDIO;
	node->status.st_rdev = DEVID_STDIO;
	node->status.st_ino = STDERR_FILENO;
	node->close = NULL;
	node->read = NULL;
	node->write = &write_stderr;
	node->creat = NULL;
	node->open = &open_stderr;
	node->lseek = NULL;

	cur_task->files[STDERR_FILENO].vfs_node = node;
	cur_task->files[STDERR_FILENO].at = 0;
	cur_task->files[STDERR_FILENO].oflag = O_WRONLY;

	return STDERR_FILENO;
}

static ssize_t write_stderr(int fd, const void* buf, size_t size)
{
	(void)fd;

	for (size_t i = 0; i < size; i++)
		kputchar(((char*)buf)[i]);

	return (ssize_t)size;
}

/* Only used for when opening as fd 2 */
static int open_stderr(struct vfs_node_t* node, int oflag, mode_t mode)
{
	(void)mode;
	(void)oflag;
	(void)node;

	int ret = STDERR_FILENO;

	/* If STDERR_FILENO is not free, get the first free fd */
	if (cur_task->files[STDERR_FILENO].vfs_node != NULL)
		ret = get_free_fd(3);

	if (ret < 0)
		errno = EMFILE;

	return ret;
}
