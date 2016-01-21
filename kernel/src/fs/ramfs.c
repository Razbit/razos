/* This file is a part of the RazOS project
 *
 * ramfs.h -- RAM File System
 *
 * Razbit 2015, 2016 */

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <kmalloc.h>
#include <errno.h>

#include "../mm/task.h"

#include "vfs.h"
#include "ramfs.h"

struct ramfs_data_t* ramfs_nodes[] = {NULL};
uint32_t ramfs_inodes = 0;

ssize_t read_ramfs(int fd, void* buf, size_t size)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	
   	size_t start_node = cur_task->files[fd].at / 0xFF;
	size_t offset = cur_task->files[fd].at % 0x100;
	size_t readable = (size_t)node->status.st_size - cur_task->files[fd].at;

	struct ramfs_data_t* curnode = ramfs_nodes[node->status.st_ino];

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

ssize_t write_ramfs(int fd, const void* buf, size_t size)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	size_t start_node = cur_task->files[fd].at / 0xFF;
	size_t offset = cur_task->files[fd].at % 0x100;

	struct ramfs_data_t* curnode = ramfs_nodes[node->status.st_ino];

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
			node->status.st_size++;
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
	    
		memset(&(curnode->data[0]), 0, 256);
		curnode->next = NULL;
		offset = 0;
	}

	return written;
}

int creat_ramfs(struct vfs_node_t* node, uint32_t mode)
{
	(void)mode;
	
	/* Set up function pointers. Creat is set in creat_vfs */
	node->read = &read_ramfs;
	node->write = &write_ramfs;
	node->open = NULL;
	node->close = NULL;
	node->lseek = NULL;

	/* Create the ramfs node */
	ramfs_nodes[node->status.st_ino] = \
		(struct ramfs_data_t*)kmalloc(sizeof(struct ramfs_data_t));
	if (ramfs_nodes[node->status.st_ino] == NULL)
	{
		errno = ENOSPC;
		return -1;
	}
	
	memset(&(ramfs_nodes[node->status.st_ino]->data[0]), 0, 256);

	return 1;
}
