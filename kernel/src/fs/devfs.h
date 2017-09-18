/* devfs.h -- device file system */

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

#ifndef DEVFS_H
#define DEVFS_H

#include "device.h"
#include "vfs.h"

#include <sys/types.h>

extern struct devfs_node_t* devfs_root;

struct devfs_node_t
{
	char* path;
	struct device_t* dev;
	struct devfs_node_t* next;
};

int devfs_init();
int devfs_exist(char* path, void* dev);
int devfs_open(char* path, int oflag, mode_t mode, struct fildes_t* fildes);

#endif /* DEVFS_H */
