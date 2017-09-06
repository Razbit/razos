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


#ifndef DEVICE_H
#define DEVICE_H

#include <sys/types.h>

/* dev_t: uint32_t
 * lower 16 bits minor, higher 16 major */

#define DEV_MAJ 0xFFFF0000
#define DEV_MIN 0x0000FFFF

/* Special devices */
#define DEV_SPECIAL_MAJ 0x00010000
#define DEVID_RAMFS (DEV_SPECIAL_MAJ | 1)
#define DEVID_PIPE  (DEV_SPECIAL_MAJ | 2)
#define DEVID_STDIO (DEV_SPECIAL_MAJ | 3)

typedef enum __dev_type
{
	DEV_UNKOWN = 0,
	DEV_CHAR = 1,
	DEV_BLOCK = 2
} dev_type_t;

typedef int (*dev_read_t)(void*, off_t, size_t, struct device_t*);
typedef int (*dev_write_t)(void*, off_t, size_t, struct device_t*);

struct device_t
{
	char name[64];
	dev_t devid;
	dev_type_t type;
	struct fs_t* fs;
	dev_read_t dev_read;
	dev_write_t dev_write;
	void* private;          // for use of the device driver
};

struct mount_t
{
	char* path;
	struct device_t* device;
	struct mount_t* next;
};

extern struct mount_t* mount_list;

void dev_init();
int dev_add(struct device_t* dev);
struct device_t* dev_get(dev_t id);

int dev_mount(struct device_t* dev, char* path);
int traverse_path(char* path);

#endif /* DEVICE_H */
