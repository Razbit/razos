/* Copyright (c) 2015 - 2017 Eetu "Razbit" Pesonen
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
#include <unistd.h>
#include <fcntl.h>
#include <console.h>
#include <string.h>
#include <kmalloc.h>
#include <errno.h>

#include "../mm/task.h"

#include "vfs.h"
#include "device.h"
#include "stdio_vfs.h"

static ssize_t write_stderr(int fd, char* path, const void* buf, size_t size, struct device_t* dev);
static int open_stderr(char* path, int oflag, mode_t mode, struct fildes_t* fildes);

int init_stderr()
{
	kprintf("Initializing stderr..\n");

	/* stderr already open? */
	if (cur_task->files[STDERR_FILENO].sysflag & FD_USED)
	{
		if (cur_task->files[STDERR_FILENO].dev->devid == DEVID_STDERR)
			return STDERR_FILENO;
	}

	cur_task->files[STDERR_FILENO].status = kmalloc(sizeof(struct stat));
	if (cur_task->files[STDERR_FILENO].status == NULL)
	{
		return -1;
	}


	struct fs_t* fs = kmalloc(sizeof(struct fs_t));
	if (fs == NULL)
		return -1;

	fs->write = write_stderr;
	fs->open = open_stderr;
	strcpy(fs->type, "stderr");

	struct device_t* stderr_dev = kmalloc(sizeof(struct device_t));
	if (stderr_dev == NULL)
	{
		kfree(fs);
		return -1;
	}

	strcpy(stderr_dev->name, "stderr");
	stderr_dev->devid = DEVID_STDERR;
	stderr_dev->type = DEV_CHAR;
	stderr_dev->fs = fs;

	if (dev_add(stderr_dev) < 0)
	{
		kfree(fs);
		kfree(stderr_dev);
		return -1;
	}

	cur_task->files[STDERR_FILENO].path = kmalloc(strlen("/dev/stderr")+1);
	if (cur_task->files[STDERR_FILENO].path == NULL)
	{
		/* TODO: remove dev */
		kfree(fs);
		kfree(stderr_dev);
		return -1;
	}

	strcpy(cur_task->files[STDERR_FILENO].path, "/dev/stderr");
	cur_task->files[STDERR_FILENO].at = 0;
	cur_task->files[STDERR_FILENO].oflag = O_WRONLY;
	cur_task->files[STDERR_FILENO].sysflag = FD_USED;
	cur_task->files[STDERR_FILENO].dev = stderr_dev;

	return STDERR_FILENO;
}


static ssize_t write_stderr(int fd, char* path, const void* buf, size_t size, struct device_t* dev)
{
	(void)fd;
	(void)path;
	(void)dev;

	for (size_t i = 0; i < size; i++)
		kputchar(((char*)buf)[i]);

	return (ssize_t)size;
}


static int open_stderr(char* path, int oflag, mode_t mode, struct fildes_t* fildes)
{
	(void)path;
	(void)mode;
	(void)oflag;

	fildes->dev = dev_get(DEVID_STDERR);

	int ret = STDERR_FILENO;

	/* If STDERR_FILENO is not free, get the first free fd */
	if (cur_task->files[STDERR_FILENO].sysflag & FD_USED)
		ret = get_free_fd(3);

	if (ret < 0)
		errno = EMFILE;

	return ret;
}
