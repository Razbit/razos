/* devfs.c -- device file system */

/* Copyright (c) 2017 Eetu "Razbit" Pesonen
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

#include "devfs.h"
#include "device.h"
#include "vfs.h"

#include <sys/types.h>
#include <kmalloc.h>
#include <string.h>
#include <console.h>

struct devfs_node_t* devfs_root = NULL;

int devfs_init()
{
	kprintf("Initializing devfs..\n");
	if (dev_init() < 0)
		return -1;

	/* create devfs */
	struct fs_t* fs = kmalloc(sizeof(struct fs_t));
	if (fs == NULL)
		return -1;

	devfs_root = kmalloc(sizeof(struct devfs_node_t));
	if (devfs_root == NULL)
	{
		kfree(fs);
		return -1;
	}

	fs->read = 0;
	fs->write = 0;
	fs->close = 0;
	fs->lseek = 0;
	fs->open = devfs_open;
	fs->creat = 0;
	fs->exist = devfs_exist;
	fs->mount = 0;

	strcpy(&(fs->type[0]), "devfs");

	struct device_t* devdev = kmalloc(sizeof(struct device_t));
	if (devdev == NULL)
	{
		kfree(fs);
		kfree(devfs_root);
		devfs_root = NULL;
		return -1;
	}

	strcpy(&(devdev->name[0]), "devdev");
	devdev->devid = DEVID_DEVDEV;
	devdev->type = DEV_CHAR;
	devdev->fs = fs;
	devdev->dev_read = NULL;
	devdev->dev_write = NULL;
	devdev->private = NULL;

	dev_add(devdev);

	if (dev_mount(devdev, "/dev") < 0)
		return -1;

	return 0;
}

int devfs_exist(char* path, void* _fildes)
{
	struct fildes_t* fildes = _fildes;

	if (path == NULL)
		return 0;

	if (path[0] == '/')
		path += 1;

	struct devfs_node_t* node = devfs_root;

	while(node)
	{
		if (node->path)
		{
			if (strcmp(path, node->path) == 0)
			{
				fildes->dev = node->dev;
				return 1;
			}

		}

		node = node->next;
	}
	return 0;
}

int devfs_open(char* path, int oflag, mode_t mode, struct fildes_t* fildes)
{
	(void)oflag;
	(void)mode;

	if (path == NULL)
		return -1;

	if (path[0] == '/')
		path += 1;

	struct devfs_node_t* node = devfs_root;

	while(node)
	{
		if (node->path)
		{
			if (strcmp(path, node->path) == 0)
				break;
		}

		node = node->next;
	}

	if (node == NULL)
		return -1;

	return get_free_fd(3);
}
