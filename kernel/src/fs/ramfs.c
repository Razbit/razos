/* This file is a part of the RazOS project
 *
 * ramfs.h -- RAM File System, the (currently) only
 * filesystem implementation (not to be confused with initrd)
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <kmalloc.h>
#include "../mm/task.h"

#include "vfs.h"
#include "ramfs.h"

struct ramfs_data_t* ramfs_nodes[] = {NULL};
uint32_t ramfs_inodes = 0;

ssize_t read_ramfs(int fd, void* buf, size_t size)
{
	if (size == 0)
		return 0;
	if (buf == NULL)
		return -1;

	struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	
	if (node == NULL)
		return -1;

	size_t start_node = cur_task->files[fd].at / 0xFF;
	size_t offset = cur_task->files[fd].at % 0x100;
	size_t readable = node->size - cur_task->files[fd].at;

	struct ramfs_data_t* curnode = \
		ramfs_nodes[node->inode - RAMFS_OFFSET];
	size_t i = 0;
	for (i = 0; i < start_node; i++)
	{
		curnode = curnode->next;
	}

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
	if (size == 0)
		return 0;
	if (buf == NULL)
		return -1;

	struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	
	if (node == NULL)
		return -1;

	size_t start_node = cur_task->files[fd].at / 0xFF;
	size_t offset = cur_task->files[fd].at % 0x100;

	struct ramfs_data_t* curnode = \
		ramfs_nodes[node->inode - RAMFS_OFFSET];
	size_t i = 0;
	for (i = 0; i < start_node; i++)
	{
		curnode = curnode->next;
	}

	/* Now that we know where are we gonna write at, we write */
	size_t written = 0;
	while (written < size)
	{
		for (; offset < 256; offset++)
		{
			curnode->data[offset] = *(uint8_t*)(buf+written);
			written++;
			node->size++;
			cur_task->files[fd].at++;
			if (written >= size)
				break;
		}

		/* We filled a node -> allocate a new one */
		curnode->next = \
			(struct ramfs_data_t*)kmalloc(sizeof(struct ramfs_data_t));
		curnode = curnode->next;

		if (curnode == NULL)
			break;
	    
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
	/* The inodes 0..1023 are reserved for ramfs use */
	ramfs_nodes[node->inode - RAMFS_OFFSET] = \
		(struct ramfs_data_t*)kmalloc(sizeof(struct ramfs_data_t));
	if (ramfs_nodes[node->inode - RAMFS_OFFSET] == NULL)
		return -1;
	
	memset(&(ramfs_nodes[node->inode - RAMFS_OFFSET]->data[0]), 0, 256);

	return 1;
}
