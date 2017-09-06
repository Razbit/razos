/* device.h -- devices */

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
#include "vfs.h"
#include <sys/types.h>
#include <string.h>

struct mount_t* mount_list;

void dev_init()
{

}

int dev_add(struct device_t* dev)
{
	return 0;
}

struct device_t* dev_get(dev_t id)
{
	return (void*)0;
}

int dev_mount(struct device_t* dev, char* path)
{
	return 0;
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
