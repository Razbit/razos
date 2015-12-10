/* This file is a part of the RazOS project
 *
 * vfs.h -- The kernel Virtual File System
 *
 * Razbit 2014, 2015 */

#include "vfs.h"
#include "../mm/task.h"

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
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

int open_vfs(const char* name, int oflag)
{
	/* Find the corresponding VFS node for the file */
	struct vfs_node_t* node = NULL;
	struct vfs_node_t* ptr = vfs_root;
	while (ptr != NULL && strncmp(name, &(ptr->name[0]), 64) != 0)
	{
		ptr = ptr->next;
	}

	if (ptr == NULL)
	{
		int ret = creat_vfs(name, VFS_FILE);
		cur_task->files[ret].at = 0;
		cur_task->files[ret].oflag = oflag;
		return ret;
	}
	
	node = ptr;

    /* Use fs-provided open(), if available */
    if (node->open != NULL)
        node->open(node, oflag);

    
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

int creat_vfs(const char* name, uint32_t mode)
{
	struct vfs_node_t* node = vfs_root;
	
	/* First file */
	if (node == NULL)
	{
		node = (struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
	}
	else
	{
		while (node->next != NULL)
			node = node->next;

		node->next = \
			(struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		node = node->next;
	}
	
	node->flags = mode;
	node->next = NULL;
	strncpy(&(node->name[0]), name, 63);
	node->name[63] = '\0';
	node->inode = inodes++;
	node->size = 0;

	/* One day, when we have multiple file systems, we'll have to figure
	 * out onto which fs we put this file on. Now use just ramfs */
	if (mode & VFS_FILE)
		node->creat = &creat_ramfs;
	else
		node->creat = NULL;
		
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
			cur_task->files[fd].at = node->size + offset;
			break;
		default:
			return (off_t)-1;
		}
		return cur_task->files[fd].at;
	}
}

