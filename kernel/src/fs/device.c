/* device.c -- devices */

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

#include "device.h"
#include "devfs.h"
#include "vfs.h"
#include <sys/types.h>
#include <string.h>
#include <kmalloc.h>
#include <console.h>


struct mount_t* mount_list = NULL;
struct device_t* devices = NULL;
dev_t last_devid = 0;

int dev_init()
{
	if (mount_list)
		return 0;

	kprintf("Initializing device handler..\n");

	devices = kmalloc(sizeof(struct device_t));
	if (devices == NULL)
		return -1;

	last_devid = 0;

	return 0;
}

int dev_add(struct device_t* dev)
{
	kprintf("Adding device %s\n", dev->name);
	struct device_t* device = devices;

	while(device->next)
		device = device->next;

	device->next = dev;
	last_devid++;

	/* Add to devfs, too */
	if (devfs_root && (dev->devid != DEVID_DEVDEV))
	{
		struct devfs_node_t* node = devfs_root;
		while (node->next)
			node = node->next;

		node->next = kmalloc(sizeof(struct devfs_node_t));
		if (node->next == NULL)
			return -1;

		node = node->next;

		node->path = dev->name;
		node->dev = dev;
		node->next = NULL;
	}

	return last_devid;
}

struct device_t* dev_get(dev_t id)
{
	struct device_t* device = devices;

	while ((device != NULL) && (device->devid != id))
		device = device->next;

	return device;
}

int dev_mount(struct device_t* dev, char* path)
{
	kprintf("Mounting %s on %s\n", dev->name, path);

	struct mount_t* mount;

	if (mount_list == NULL)
	{
		mount_list = kmalloc(sizeof(struct mount_t));
		if (mount_list == NULL)
			return -1;
		mount = mount_list;

		mount->path = kmalloc(strlen(path)+1);
		if (mount->path == NULL)
		{
			kfree(mount);
			mount_list = NULL;
			return -1;
		}
	}
	else
	{
		mount = mount_list;

		for(;;)
		{
			if (mount == NULL)
				break;

			/* if mount point is in use already */
			if (strcmp(mount->path, path) == 0)
				return -1;

			mount = mount->next;
		}
		mount = mount_list;
		while (mount->next)
			mount = mount->next;

		mount->next = kmalloc(sizeof(struct mount_t));
		if (mount->next == NULL)
			return -1;

		mount->next->path = kmalloc(strlen(path)+1);
		if (mount->next->path == NULL)
		{
			kfree(mount->next);
			mount->next = NULL;
			return -1;
		}

		mount = mount->next;

	}

	strcpy(mount->path, path);
	mount->device = dev;
	mount->next = NULL;

	if (dev->fs->mount != NULL)
	{
		if (dev->fs->mount(path, dev) < 0)
		{
			kfree(mount->path);
			if (mount->next == NULL)
			{
				kfree(mount);
				mount_list = NULL;
			}
			else
			{
				mount = mount_list;
				while (mount->next->next)
					mount = mount->next;
				kfree(mount->next);
				mount->next = NULL;
			}

			return -1;
		}
	}

	return 0;
}

void list_mount()
{
	struct mount_t* mount = mount_list;

	while (mount)
	{
		kprintf("%s (%8.8x) on %s\n", &(mount->device->name[0]), \
		        mount->device->devid, mount->path);
		mount = mount->next;
	}
}

/* Traverse by removing the last '/' */
int traverse_path(char* path)
{
	int len = (int)strlen(path);
	len--;

	while (len >= 0)
	{
		if (path[len] == '/')
		{
			path[len] = 0;
			break;
		}
		len--;
	}

	return len;
}

char* reduce_path(const char* path, const char* mount)
{
	/* remove <mount> from the beginning of <path> */
	return (char*)((size_t)path + strlen(mount));
}
