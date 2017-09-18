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

static ssize_t write_stdout(int fd, char* path, const void* buf, size_t size, struct device_t* dev);
static int open_stdout(char* path, int oflag, mode_t mode, struct fildes_t* fildes);


int init_stdout()
{
	kprintf("Initializing stdout..\n");

	/* stdout already open? */
	if (cur_task->files[STDOUT_FILENO].sysflag & FD_USED)
	{
		if (cur_task->files[STDOUT_FILENO].dev->devid == DEVID_STDOUT)
			return STDOUT_FILENO;
	}

	cur_task->files[STDOUT_FILENO].status = kmalloc(sizeof(struct stat));
	if (cur_task->files[STDOUT_FILENO].status == NULL)
	{
		return -1;
	}

	struct fs_t* fs = kmalloc(sizeof(struct fs_t));
	if (fs == NULL)
		return -1;

	fs->write = write_stdout;
	fs->open = open_stdout;
	strcpy(fs->type, "stdout");

	struct device_t* stdout_dev = kmalloc(sizeof(struct device_t));
	if (stdout_dev == NULL)
	{
		kfree(fs);
		return -1;
	}

	strcpy(stdout_dev->name, "stdout");
	stdout_dev->devid = DEVID_STDOUT;
	stdout_dev->type = DEV_CHAR;
	stdout_dev->fs = fs;

	if (dev_add(stdout_dev) < 0)
	{
		kfree(fs);
		kfree(stdout_dev);
		return -1;
	}

	cur_task->files[STDOUT_FILENO].path = kmalloc(strlen("/dev/stdout")+1);
	if (cur_task->files[STDOUT_FILENO].path == NULL)
	{
		/* TODO: remove dev */
		kfree(fs);
		kfree(stdout_dev);
		return -1;
	}

	strcpy(cur_task->files[STDOUT_FILENO].path, "/dev/stdout");
	cur_task->files[STDOUT_FILENO].at = 0;
	cur_task->files[STDOUT_FILENO].oflag = O_WRONLY;
	cur_task->files[STDOUT_FILENO].sysflag = FD_USED;
	cur_task->files[STDOUT_FILENO].dev = stdout_dev;

	return STDOUT_FILENO;
}

static ssize_t write_stdout(int fd, char* path, const void* buf, size_t size, struct device_t* dev)
{
	(void)fd;
	(void)path;
	(void)dev;

	for (size_t i = 0; i < size; i++)
		kputchar(((char*)buf)[i]);

	return (ssize_t)size;
}


static int open_stdout(char* path, int oflag, mode_t mode, struct fildes_t* fildes)
{
	(void)path;
	(void)mode;
	(void)oflag;

	fildes->dev = dev_get(DEVID_STDOUT);

	int ret = STDOUT_FILENO;

	/* If STDOUT_FILENO is not free, get the first free fd */
	if (cur_task->files[STDOUT_FILENO].sysflag & FD_USED)
		ret = get_free_fd(3);

	if (ret < 0)
		errno = EMFILE;

	return ret;
}
