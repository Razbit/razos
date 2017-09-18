/* ramfs.c -- RAM File System */

/* Copyright (c) 2015-2017 Eetu "Razbit" Pesonen
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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <kmalloc.h>
#include <errno.h>
#include <asm/system.h>

#include "../mm/task.h"

#include "vfs.h"
#include "ramfs.h"

struct ramfs_hdr_t* ramfs_nodes = NULL;
uint32_t ramfs_inodes = 0;

int ramfs_init()
{
	/* create ramfs */
	struct fs_t* fs = kmalloc(sizeof(struct fs_t));
	if (fs == NULL)
		return -1;

	fs->read = read_ramfs;
	fs->write = write_ramfs;
	fs->creat = creat_ramfs;
	fs->exist = exist_ramfs;

	strcpy(fs->type, "ramfs");

	struct device_t* ramdisk = kmalloc(sizeof(struct device_t));
	if (ramdisk == NULL)
	{
		kfree(fs);
		return -1;
	}

	strcpy(ramdisk->name, "ramdisk");
	ramdisk->devid = DEVID_RAMDISK;
	ramdisk->type = DEV_CHAR;
	ramdisk->fs = fs;

	if (dev_add(ramdisk) < 0)
	{
		kfree(fs);
		kfree(ramdisk);
		return -1;
	}

	if (dev_mount(ramdisk, "/rd") < 0)
		return -1;

	return 0;
}

ssize_t read_ramfs(int fd, char* path, void* buf, size_t size, struct device_t* dev)
{
	(void)dev;
	(void)path;

   	size_t start_node = cur_task->files[fd].at / 0xFF;
	size_t offset = cur_task->files[fd].at % 0x100;
	size_t readable = (size_t)cur_task->files[fd].status->st_size - cur_task->files[fd].at;

	struct ramfs_hdr_t* curhdr = ramfs_nodes;

	while (ramfs_nodes)
	{
		if (curhdr->status.st_ino == cur_task->files[fd].status->st_ino)
			break;
		curhdr = curhdr->next;
	}

	if (curhdr == NULL)
	{
		errno = EBADF;
		return -1;
	}

	struct ramfs_data_t* curnode = curhdr->data;

	for (size_t i = 0; i < start_node; i++)
		curnode = curnode->next;

	/* Now that we know where are we gonna read at, we read */
	if (size > readable)
		size = readable;

	size_t read = 0;
	while (read < size)
	{
		for (; offset < 256; offset++)
		{
			/* Stop if we are at EOF */
			if (read >= size)
				goto exit;
			*(uint8_t*)(buf+read) = curnode->data[offset];
			read++;
			cur_task->files[fd].at++;
		}

		curnode = curnode->next;
		offset = 0;
	}

exit:
	return read;
}

ssize_t write_ramfs(int fd, char* path, const void* buf, size_t size, struct device_t* dev)
{
	(void)path;
	(void)dev;

	size_t start_node = cur_task->files[fd].at / 0xFF;
	size_t offset = cur_task->files[fd].at % 0x100;

	struct ramfs_hdr_t* curhdr = ramfs_nodes;

	while(curhdr)
	{
		if (curhdr->status.st_ino == cur_task->files[fd].status->st_ino)
			break;
		curhdr = curhdr->next;
	}

	if (curhdr == NULL)
	{
		errno = EBADF;
		return -1;
	}
	struct ramfs_data_t* curnode = curhdr->data;

	for (size_t i = 0; i < start_node; i++)
		curnode = curnode->next;

	/* Now that we know where are we gonna write at, we write */
	size_t written = 0;
	while (written < size)
	{
		for (; offset < 256; offset++)
		{
			curnode->data[offset] = *(uint8_t*)(buf+written);
			written++;
			cur_task->files[fd].status->st_size++;
			curhdr->status.st_size++;
			cur_task->files[fd].at++;
			if (written >= size)
				break;
		}

		/* We filled a node -> allocate a new one */
		curnode->next = \
			(struct ramfs_data_t*)kmalloc(sizeof(struct ramfs_data_t));
		curnode = curnode->next;

		if (curnode == NULL)
		{
			if (written == 0) /* if couldn't write anything, fail */
				return -1; /* kmalloc sets errno */
			else
				break;
		}

		curnode->next = NULL;
		offset = 0;
	}

	return written;
}

int creat_ramfs(char* path, uint32_t mode, struct device_t* dev, struct fildes_t* fildes)
{
	(void)mode;
	(void)dev;

	fildes->status->st_ino = ramfs_inodes++;

	/* Create the ramfs node */
	struct ramfs_hdr_t* header = kmalloc(sizeof(struct ramfs_hdr_t));
	if (header == NULL)
	{
		errno = ENOSPC;
		return -1;
	}

	header->name = kmalloc(strlen(path));
	strcpy(header->name, path);
	header->status.st_ino = fildes->status->st_ino;
	header->next = NULL;
	header->data = kmalloc(sizeof(struct ramfs_data_t));

	if (header->data == NULL)
	{
		kfree(header);
		errno = ENOSPC;
		return -1;
	}

	/* add to the list */
	if (ramfs_nodes == NULL)
		ramfs_nodes = header;
	else
	{
		struct ramfs_hdr_t* node = ramfs_nodes;
		while (node->next)
			node = node->next;
		node->next = header;
	}

	return 1;
}

int exist_ramfs(char* path, void* _fildes)
{
	struct fildes_t* fildes = _fildes;
	struct ramfs_hdr_t* node = ramfs_nodes;

	if (node == NULL)
		return 0;

	while (strcmp(path, node->name) != 0)
	{
		node = node->next;
		if (node == NULL)
			return 0;
	}

	/* We found the node we are looking for */
	*(fildes->status) = node->status;
	return 1;
}

int close_ramfs(int fd, char* path, struct device_t* device)
{
	(void)fd;
	(void)path;
	(void)device;

	return 0;
}
