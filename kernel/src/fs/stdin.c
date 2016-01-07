/* This file is a part of the RazOS project
 *
 * stdin.c -- standard input
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <console.h> /* For now, a better terminal on the way */
#include <kmalloc.h>

#include "../mm/task.h"
#include "../mm/sched.h"

#include "vfs.h"
#include "ramfs.h"
#include "stdio_vfs.h"

static ssize_t read_stdin(int fd, void* buf, size_t size);
static ssize_t write_stdin(int fd, const void* buf, size_t size);
static int open_stdin(struct vfs_node_t* node, int oflag, mode_t mode);
	
static off_t read_at = 0;
static off_t write_at = 0;

int init_stdin()
{
	int fd = open_vfs("stdin", O_RDWR | O_CREAT, S_IFREG);

	/* Now that we have a file in ramfs, let's modify the vfs_node */
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	node->write = &write_stdin;
	node->read = &read_stdin;
	node->open = &open_stdin;

	close_vfs(fd);

	return 1;
}

static ssize_t read_stdin(int fd, void* buf, size_t size)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	/* Wait for user input */
	while (node->status.st_size < size)
		sched_switch();

	lseek_vfs(fd, read_at, SEEK_SET);
	size_t read = read_ramfs(fd, buf, size);
	
	node->status.st_size -= read;
	read_at += read;

	/* If there is an empty ramfs data node, free() it */
	while (read_at >= 256)
	{
		struct ramfs_data_t* ramfsnode = \
			ramfs_nodes[node->status.st_ino];
		ramfs_nodes[node->status.st_ino] = ramfsnode->next;
		read_at -= 256;
		kfree(ramfsnode);
	}

	return read;
}

static ssize_t write_stdin(int fd, const void* buf, size_t size)
{
	lseek_vfs(fd, write_at, SEEK_SET);
	write_at += write_ramfs(fd, buf, size);

	/* Echo to the screen */
	size_t i = 0;
	for (; i < size; i++)
	{
		kputchar(((char*)buf)[i]);
	}
    
	return size;
}

/* Only used for when opening as fd 0 */
static int open_stdin(struct vfs_node_t* node, int oflag, mode_t mode)
{
	(void)mode;
	(void)oflag;
	(void)node;

	/* If STDIN_FILENO is not free, get the first free fd */
	if (cur_task->files[STDIN_FILENO].vfs_node != NULL)
		return get_free_fd(3);

	return STDIN_FILENO;
}
