/* This file is a part of the RazOS project
 *
 * vfs.c -- The kernel Virtual File System
 *
 * Razbit 2014, 2015, 2016 */

#include "vfs.h"
#include "../mm/task.h"
#include "ramfs.h"
#include "fifofs.h"

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <kmalloc.h>
#include <limits.h>

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
			if (ptr->status.st_dev == DEVID_PIPE)
				ptr = ptr->next;
			else if (strncmp(name, &(ptr->name[0]), strlen(name)) == 0)
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
			if (ret < 0)
				return -1;
			return ret;
		}
		else
			return -1;
	}

	node = ptr;

	int fd = 3; /* 0, 1, 2 reserved for stdin/out/err */
	/* Use fs-provided open(), if available */
	if (node->open != NULL)
	{
		fd = node->open(node, oflag, mode);
	}
	else
	{
		/* Find an empty fildes from the task struct */
		fd = get_free_fd(fd);
	}

	if (fd >= 0)
	{
		/* Populate the fildes */
		cur_task->files[fd].vfs_node = node;
		cur_task->files[fd].at = 0;
		cur_task->files[fd].oflag = oflag;

		return fd;
	}
	else
	{
		return -1;
	}
}

int close_vfs(int fd)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	if (node == NULL)
	{
		/* TODO: set errno to EBADF */
		return -1;
	}

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
		if (node == NULL)
			return -1;

		vfs_root = node;
	}
	else
	{
		while (node->next != NULL)
			node = node->next;

		node->next = \
			(struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		if (node->next == NULL)
			return -1;

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

	/* This will be better when we have mounts and dirs, this is
	 * rather stupid */
	/* if this is a regular file or type not specified */
	if ((mode & S_IFREG) || (mode & S_IFMT) == 0)
	{
		node->status.st_dev = DEVID_RAMFS;
		node->status.st_rdev = DEVID_RAMFS;
		node->status.st_ino = ramfs_inodes++;
		node->creat = &creat_ramfs;
	}
	else if (mode & S_IFIFO)
	{
		/* fifos created on ramfs for now */
		node->status.st_dev = DEVID_RAMFS;
		node->status.st_rdev = DEVID_RAMFS;
		node->status.st_ino = ramfs_inodes++;
		node->creat = &creat_fifofs;
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
	int fd = get_free_fd(3);

	/* Now i is  the first free index. Populate the fildes */
	cur_task->files[fd].vfs_node = node;
	cur_task->files[fd].at = 0;
	cur_task->files[fd].oflag = O_WRONLY;

	return fd;
}

off_t lseek_vfs(int fd, off_t offset, int whence)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	/* FIFO not seekable */
	if (node->status.st_mode & S_IFIFO)
		return cur_task->files[fd].at;

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

/* Find first free file descriptor, starting from fd */
int get_free_fd(int fd)
{
	if (fd < 0)
		return -1;
	
	while (1)
	{
		if (fd == OPEN_MAX) /* No free file descriptors.. */
		{
			return -1;
		}

		if (cur_task->files[fd].vfs_node != NULL)
		{
			fd++;
			continue;
		}

		break;
	}

	return fd;
}
