/* This file is a part of the RazOS project
 *
 * vfs.h -- The kernel Virtual File System
 *
 * Razbit 2014, 2015 */

#include "vfs.h"
#include "../mm/task.h"
#include "ramfs.h"

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <kmalloc.h>
#include <console.h>

/* Root of the filesystem */
struct vfs_node_t* vfs_root = NULL;
uint32_t inodes = 0;

ssize_t read_vfs(int fd, void* buf, size_t size)
{
	uint32_t access = cur_task->files[fd].oflag & O_ACCMODE;
	if (access == O_RDWR || access == O_RDONLY)
	{
		struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	
		if (node->read != NULL)
			return node->read(fd, buf, size);
		else
			return -1;
	}
	else
		return -1;
}

ssize_t write_vfs(int fd, const void* buf, size_t size)
{
	uint32_t access = cur_task->files[fd].oflag & O_ACCMODE;
	if (access == O_RDWR || access == O_WRONLY)
	{
		struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	
		if (node->write != NULL)
			return node->write(fd, buf, size);
		else
			return -1;
	}
	else
		return -1;
}

int open_vfs(const char* name, int oflag, mode_t mode)
{
	/* Find the corresponding VFS node for the file */
	struct vfs_node_t* node = NULL;
	struct vfs_node_t* ptr = vfs_root;
	
	if (vfs_root != NULL)
	{
		while (ptr != NULL)
		{			
			if (strncmp(name, &(ptr->name[0]), strlen(name)) == 0)
				break;
			else
				ptr = ptr->next;
		}
	}
    
	/* File doesn't exist yet -> create it. */
	if (ptr == NULL)
	{
		if (oflag & O_CREAT)
		{
			int ret = creat_vfs(name, mode);
			cur_task->files[ret].at = 0;
			cur_task->files[ret].oflag = oflag;
			return ret;
		}
		else
			return -1;
	}
	
	node = ptr;

	/* Use fs-provided open(), if available */
	if (node->open != NULL)
	{
		int ret = node->open(node, oflag, mode);
		if (ret >= 0 && ret <= 2) /* Return if we opened stdio */
			return ret;
	}

    
	/* Find an empty fildes from the task struct, fill it and
	 * return the index */
	int i = 3; /* 0, 1, 2 reserved for stdin/out/err */
	while (1)
	{
		if (i == 32) /* No free file descriptors.. */
		{
			return -1;
		}
	    
		if (cur_task->files[i].vfs_node != NULL)
		{
			i++;
			continue;
		}

		break;
	}

	/* Now i is  the first free index. Populate the fildes */
	cur_task->files[i].vfs_node = node;
	cur_task->files[i].at = 0;
	cur_task->files[i].oflag = oflag;

	return i;
}

int close_vfs(int fd)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	
	if (node->close != NULL)
	{
		int ret = node->close(fd);
		if (ret < 0)
			return -1;
	}

	cur_task->files[fd].vfs_node = NULL;
	cur_task->files[fd].at = 0;
	cur_task->files[fd].oflag = 0;

	return 0;
}

int creat_vfs(const char* name, mode_t mode)
{
	struct vfs_node_t* node = vfs_root;
	
	/* First file */
	if (node == NULL)
	{
		node = (struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		vfs_root = node;
	}
	else
	{
		while (node->next != NULL)
			node = node->next;

		node->next = \
			(struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		node = node->next;
	}

	/* Yeah, this is rather stupid since we ain't got no users xD */
	/* If access mode is unspecified, use 0-rwx-rwx-rwx */
	if (mode & S_IPERM == 0)
		mode |= (S_IRWXU | S_IRWXG | S_IRWXO);

	node->status.st_mode = mode;
	node->status.st_uid = 0;
	node->status.st_gid = 0;
	
	node->next = NULL;
	strncpy(&(node->name[0]), name, 63);
	node->name[63] = '\0';
	node->status.st_size = 0;

	/* if this is a regular file or type not specified */
	if ((mode & S_IFREG) || (mode & S_IFMT) == 0)
	{
		/* Create it in ramfs (for now, with dirs and mounts this
		 * will be better ) */
		node->status.st_dev = DEVID_RAMFS;
		node->status.st_rdev = DEVID_RAMFS;
		node->status.st_ino = ramfs_inodes++;
		node->creat = &creat_ramfs;
	}
	else
	{
		/* There is no *default* but ramfs, so just fail if we somehow
		 * come here.. */
		return -1;
	}
		
	if (node->creat != NULL)
	{
		int ret = node->creat(node, mode);
		if (ret < 0)
			return -1;
	}

	/* Find an empty fildes from the task struct, fill it and
	 * return the index */
	int i = 3; /* 0, 1, 2 reserved for stdin/out/err */
	while (1)
	{
		if (i == 32) /* No free file descriptors.. */
		{
			return -1;
		}
	    
		if (cur_task->files[i].vfs_node != NULL)
		{
			i++;
			continue;
		}

		break;
	}

	/* Now i is  the first free index. Populate the fildes */
	cur_task->files[i].vfs_node = node;
	cur_task->files[i].at = 0;
	cur_task->files[i].oflag = O_WRONLY;

	return i;
}

off_t lseek_vfs(int fd, off_t offset, int whence)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	/* If there is some fs-special functionality, use that */
	if (node->lseek != NULL)
		return node->lseek(fd, offset, whence);
	/* Otherwise we go default */
	else
	{
		switch (whence)
		{
		case SEEK_SET:
			cur_task->files[fd].at = offset;
			break;
		case SEEK_CUR:
			cur_task->files[fd].at += offset;
			break;
		case SEEK_END:
			cur_task->files[fd].at = node->status.st_size + offset;
			break;
		default:
			return (off_t)-1;
		}
		
		return cur_task->files[fd].at;
	}
}

