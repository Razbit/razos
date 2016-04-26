/* Copyright (c) 2015, 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#include <sys/types.h>
#include <asm/system.h>
#include <unistd.h>
#include <fcntl.h>
#include <console.h> /* For now, a better terminal on the way */
#include <string.h>
#include <kmalloc.h>
#include <errno.h>

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
static volatile size_t stdin_buf_size = 0;

static char stdin_buf[1024] = {'\0'};

int init_stdin()
{
	/* stdin already open? */
	if (cur_task->files[STDIN_FILENO].vfs_node != NULL)
		return STDIN_FILENO;

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
	strcpy(node->name, "stdin");

	node->status.st_dev = DEVID_STDIO;
	node->status.st_rdev = DEVID_STDIO;
	node->status.st_ino = STDIN_FILENO;
	node->close = NULL;
	node->read = &read_stdin;
	node->write = &write_stdin;
	node->creat = NULL;
	node->open = &open_stdin;
	node->lseek = NULL;

	cur_task->files[STDIN_FILENO].vfs_node = node;
	cur_task->files[STDIN_FILENO].at = 0;
	cur_task->files[STDIN_FILENO].oflag = O_RDWR;

	return STDIN_FILENO;
}

/* Put a char to the buffer */
static size_t push_stdin(char c)
{
	/* We are at top of the LIFO buffer */
	if (write_at >= 1024)
	{
		if (read_at == 0) /* We haven't read from the buf yet -> fail */
			return 0;
		write_at = 0;
	}

	stdin_buf[write_at] = c;
	write_at++;
	return 1;
}

/* Get a char from the buffer */
static char pop_stdin()
{
	if (read_at >= 1024)
		read_at = 0;
	if (read_at == write_at) /* buf empty -> fail */
		return '\0';

	read_at++;
	return stdin_buf[read_at-1];
}

static ssize_t read_stdin(int fd, void* buf, size_t size)
{
	(void)fd;
	size_t read = 0;

	sched_halt(); /* Make sure not to switch tasks now */
	sti(); /* Now we can catch kb inputs */

	while (stdin_buf_size < size);

	for (; read < size; read++)
	{
		char c = pop_stdin();
		if (c != '\0')
		{
			((char*)buf)[read] = c;
			stdin_buf_size--;
		}
	}

	sched_cont();
	return (ssize_t)read;
}

static ssize_t write_stdin(int fd, const void* buf, size_t size)
{
	(void)fd;
	for (size_t i = 0; i < size; i++)
	{
		stdin_buf_size += push_stdin(((char*)buf)[i]);

		/* Echo to the screen */
		//kputchar(((char*)buf)[i]);
	}

	return size;
}

/* Only used for when opening as fd 0 */
static int open_stdin(struct vfs_node_t* node, int oflag, mode_t mode)
{
	(void)mode;
	(void)oflag;
	(void)node;

	int ret = STDIN_FILENO;

	/* If STDIN_FILENO is not free, get the first free fd */
	if (cur_task->files[STDIN_FILENO].vfs_node != NULL)
		ret = get_free_fd(3);

	if (ret < 0)
		errno = EMFILE;

	return ret;
}
