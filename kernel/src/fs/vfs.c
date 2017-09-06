/* vfs.c -- The kernel Virtual File System */

/* Copyright (c) 2014-2016 Eetu "Razbit" Pesonen
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

#include "vfs.h"
#include "device.h"
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
#include <errno.h>

ssize_t read_vfs(int fd, void* buf, size_t size)
{
	if (buf == NULL)
	{
		errno = ENOMEM;
		return -1;
	}
	if (fd < 0 || fd >= OPEN_MAX || !(cur_task->files[fd].sysflag & FD_USED))
	{
		errno = EBADF;
		return -1;
	}
	if (cur_task->files[fd].status.st_mode & S_IFDIR)
	{
		errno = EISDIR;
		return -1;
	}
	if (size == 0)
		return 0;

	if (cur_task->files[fd].oflag & O_RDONLY)
	{
		if (cur_task->files[fd].dev->fs->read != NULL)
			return cur_task->files[fd].dev->fs->read(fd, cur_task->files[fd].path, buf, size, cur_task->files[fd].dev);
		else
		{
			errno = ENXIO;
			return -1;
		}
	}
	else
	{
		errno = EBADF;
		return -1;
	}
}

ssize_t write_vfs(int fd, const void* buf, size_t size)
{
	if (buf == NULL)
	{
		errno = ENOBUFS;
		return -1;
	}
	if (fd < 0 || fd >= OPEN_MAX || !(cur_task->files[fd].sysflag & FD_USED))
	{
		errno = EBADF;
		return -1;
	}
	if (size == 0)
		return 0;

	if (cur_task->files[fd].oflag & O_WRONLY)
	{
		if (cur_task->files[fd].write != NULL)
			return cur_task->files[fd].write(fd, cur_task->files[fd].path, buf, size, cur_task->files[fd].dev);
		else
		{
			errno = ENXIO;
			return -1;
		}
	}
	else
	{
		errno = EBADF;
		return -1;
	}
}

int open_vfs(const char* path, int oflag, mode_t mode)
{
	if (path == NULL)
	{
		errno = ENOENT;
		return -1;
	}

	/* Find the FS that the file is on by searching mount points */

	struct fildes_t* fildes = kmalloc(sizeof(struct fildes_t));
	if (fildes == NULL)
	{
		/* kmalloc sets errno */
		return -1;
	}
	struct mount_t* mount_point = mount_list;

	char* dir = kmlloc(strlen(path));
	if (dir == NULL)
	{
		kfree(fildes);
		/* kmalloc sets errno */
		return -1;
	}

	for (;;)
	{
		while (mount_point)
		{
			if (strcmp(mount_point->path, dir) == 0)
			{
				goto mount_found;
			}
			mount_point = mount_point->next;
		}

		mount_point = mount_list;

		if (traverse_path(dir) < 0)
		{
			/* getting here would be weird, we wouldn't even have / */
			errno = ENOENT;
			goto fail;
		}
	}

mount_found:

	/* do something */

	/* File doesn't exist yet -> create it. */
	if (/* doesnt exist */)
	{
		if (oflag & O_CREAT)
		{
			int ret = creat_vfs(path, mode);
			/* If creat fails, it sets errno for us */
			if (ret < 0)
				goto fail;
			cur_task->files[ret].oflag = oflag;
			if (fildes)
				kfree(fildes);
			return ret;
		}
		else
		{
			errno = ENOENT;
			goto fail;
		}
	}
	else
	{
		if ((oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL))
		{
			errno = EEXIST;
			goto fail;
		}
	}


	if ((fildes->status.st_mode & S_IFDIR) && (oflag & O_WRONLY))
	{
		errno = EISDIR;
		goto fail;
	}

	int fd = 3; /* 0, 1, 2 reserved for stdin/out/err */
	/* Use fs-provided open(), if available */
	if (fildes->open != NULL)
	{
		fd = node->open(fildes, oflag, mode);
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
		errno = EMFILE;
		goto fail;
	}

fail:
	if (fildes)
		kfree(fildes);
	if (dir)
		kfree(dir);
	return -1;
}

int close_vfs(int fd)
{
	if (fd < 0 || fd >= OPEN_MAX)
	{
		errno = EBADF;
		return -1;
	}

	if (cur_task->files[fd].sysflag & FD_USERD == 0)
	{
		errno = EBADF;
		return -1;
	}

	if (cur_task->files[fd].close != NULL)
	{
		int ret = cur_task->files[fd].close(fd, cur_task->files[fd].path);
		/* If this fails, errno should be set */
		if (ret < 0)
			return -1;
	}

	cur_task->files[fd].path = NULL;
	cur_task->files[fd].at = 0;
	cur_task->files[fd].oflag = 0;
	cur_task->files[fd].sysflag = 0;
	cur_task->files[fd].dev = NULL;

	return 0;
}

int creat_vfs(const char* name, mode_t mode)
{
	struct vfs_node_t* node = vfs_root;

	/* First file */
	if (node == NULL)
	{
		node = (struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		/* If this fails, kmalloc sets errno */
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
		/* kmalloc sets errno on failure */
		if (node->next == NULL)
			return -1;

		node = node->next;
	}

	/* Yeah, this is rather stupid since we ain't got no users xD */
	/* If access mode is unspecified, use 0-rwx-rwx-rwx */
	if ((mode & S_IPERM) == 0)
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
		errno = EROFS;
		return -1;
	}

	if (node->creat != NULL)
	{
		int ret = node->creat(node, mode);
		/* If this fails, node->creat should have set the errno */
		if (ret < 0)
			return -1;
	}

	/* Find an empty fildes from the task struct, fill it and
	 * return the index */
	int fd = get_free_fd(3);
	if (fd < 0)
	{
		errno = EMFILE;
		return -1;
	}

	/* Now i is  the first free index. Populate the fildes */
	cur_task->files[fd].vfs_node = node;
	cur_task->files[fd].at = 0;
	cur_task->files[fd].oflag = O_WRONLY;

	return fd;
}

off_t lseek_vfs(int fd, off_t offset, int whence)
{
	if (fd < 0 || fd >= OPEN_MAX)
	{
		errno = EBADF;
		return -1;
	}

	if (!(cur_task->files[fd].sysflag & FD_USED))
	{
		errno = EBADF;
		return -1;
	}

	/* FIFO not seekable */
	if (cur_task->files[fd].status.st_mode & S_IFIFO)
	{
		errno = ESPIPE;
		return -1;
	}

	/* If there is some fs-special functionality, use that. It should
	 * set errno on errors.. */
	if (cur_task->files[fd].lseek != NULL)
		return cur_task->files[fd].lseek(fd, cur_task->files[fd].path, offset, whence, cur_task->files[fd].dev);

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
			cur_task->files[fd].at = cur_task->files[fd].status.st_size + offset;
			break;
		default:
			errno = EINVAL;
			return -1;
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
			return -1;

		if (cur_task->files[fd].sysflag & FD_USED)
		{
			fd++;
			continue;
		}

		break;
	}

	return fd;
}
