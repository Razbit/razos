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
#include <console.h>
#include <string.h>
#include <kmalloc.h>
#include <errno.h>

#include "../mm/task.h"
#include "../mm/sched.h"

#include "vfs.h"
#include "device.h"
#include "stdio_vfs.h"

static ssize_t read_stdin(int fd, char* path, void* buf, size_t size, struct device_t* dev);
static ssize_t write_stdin(int fd, char* path, const void* buf, size_t size, struct device_t* dev);
static int open_stdin(char* path, int oflag, mode_t mode, struct fildes_t* fildes);

static off_t read_at = 0;
static off_t write_at = 0;
static volatile size_t stdin_buf_size = 0;

static char stdin_buf[1024] = {'\0'};

int init_stdin()
{
	kprintf("Initializing stdin..\n");

	/* stdin already open? */
	if (cur_task->files[STDIN_FILENO].sysflag & FD_USED)
	{
		if (cur_task->files[STDIN_FILENO].dev->devid == DEVID_STDIN)
			return STDIN_FILENO;
	}

	cur_task->files[STDIN_FILENO].status = kmalloc(sizeof(struct stat));
	if (cur_task->files[STDIN_FILENO].status == NULL)
	{
		return -1;
	}


	struct fs_t* fs = kmalloc(sizeof(struct fs_t));
	if (fs == NULL)
		return -1;

	fs->read = read_stdin;
	fs->write = write_stdin;
	fs->open = open_stdin;
	strcpy(fs->type, "stdin");

	struct device_t* stdin_dev = kmalloc(sizeof(struct device_t));
	if (stdin_dev == NULL)
	{
		kfree(fs);
		return -1;
	}

	strcpy(stdin_dev->name, "stdin");
	stdin_dev->devid = DEVID_STDIN;
	stdin_dev->type = DEV_CHAR;
	stdin_dev->fs = fs;

	if (dev_add(stdin_dev) < 0)
	{
		kfree(fs);
		kfree(stdin_dev);
		return -1;
	}

	cur_task->files[STDIN_FILENO].path = kmalloc(strlen("/dev/stdin")+1);
	if (cur_task->files[STDIN_FILENO].path == NULL)
	{
		/* TODO: remove dev */
		kfree(fs);
		kfree(stdin_dev);
		return -1;
	}

	strcpy(cur_task->files[STDIN_FILENO].path, "/dev/stdin");
	cur_task->files[STDIN_FILENO].at = 0;
	cur_task->files[STDIN_FILENO].oflag = O_RDWR;
	cur_task->files[STDIN_FILENO].sysflag = FD_USED;
	cur_task->files[STDIN_FILENO].dev = stdin_dev;

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

static ssize_t read_stdin(int fd, char* path, void* buf, size_t size, struct device_t* dev)
{
	(void)fd;
	(void)path;
	(void)dev;

	size_t read = 0;

	sched_halt(); /* Make sure we don't switch tasks now */
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

static ssize_t write_stdin(int fd, char* path, const void* buf, size_t size, struct device_t* dev)
{
	(void)fd;
	(void)path;
	(void)dev;

	for (size_t i = 0; i < size; i++)
	{
		stdin_buf_size += push_stdin(((char*)buf)[i]);
	}

	return size;
}


static int open_stdin(char* path, int oflag, mode_t mode, struct fildes_t* fildes)
{
	(void)path;
	(void)mode;
	(void)oflag;

	fildes->dev = dev_get(DEVID_STDIN);

	int ret = STDIN_FILENO;

	/* If STDIN_FILENO is not free, get the first free fd */
	if (cur_task->files[STDIN_FILENO].sysflag & FD_USED)
		ret = get_free_fd(3);

	if (ret < 0)
		errno = EMFILE;

	return ret;
}
