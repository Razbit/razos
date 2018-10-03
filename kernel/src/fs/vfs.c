/* vfs.c -- The kernel Virtual File System */

/* Copyright (c) 2014-2017 Eetu "Razbit" Pesonen
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
#include "pipe.h"

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <kmalloc.h>
#include <limits.h>
#include <errno.h>
#include <console.h>
#include <asm/system.h>

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

	if (cur_task->files[fd].status->st_mode & S_IFDIR)
	{
		errno = EISDIR;
		return -1;
	}

	if (size == 0)
		return 0;

	/* Handle pipes */
	if (cur_task->files[fd].sysflag & PIPE)
		return read_pipe(fd, buf, size);

	if (cur_task->files[fd].oflag & O_RDONLY)
	{
		if (cur_task->files[fd].dev->fs->read != NULL)
		{
			return cur_task->files[fd].dev->fs->read(fd, cur_task->files[fd].path, buf, size, cur_task->files[fd].dev);
		}
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

	if (cur_task->files[fd].sysflag & PIPE)
	{
		return write_pipe(fd, buf, size);
	}


	if (cur_task->files[fd].oflag & O_WRONLY)
	{
		if (cur_task->files[fd].dev->fs->write != NULL)
			return cur_task->files[fd].dev->fs->write(fd, cur_task->files[fd].path, buf, size, cur_task->files[fd].dev);
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
	/* TODO: fifo */
	if (mode & S_IFIFO)
	{
		errno = EPIPE;
		return -1;
	}

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

	fildes->status = kmalloc(sizeof(struct stat));
	if (fildes->status == NULL)
	{
		kfree(fildes);
		return -1;
	}

	struct mount_t* mount_point = mount_list;

	char* dir = kmalloc(strlen(path)+1);
	if (dir == NULL)
	{
		kfree(fildes->status);
		kfree(fildes);
		/* kmalloc sets errno */
		return -1;
	}

	strcpy(dir, path);

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

	/* mount_point now contains information of the device where
	 * the desired file is located */

	fildes->path = kmalloc(strlen(path)+1);
	if (fildes->path == NULL)
		goto fail;

	strcpy(fildes->path, path);

	fildes->dev = mount_point->device;

	/* remove path to mount point from path */
	char* reduced_path = reduce_path(path, mount_point->path);

	/* exist() populates the stat struct */
	if (!fildes->dev->fs->exist(reduced_path, fildes))
	{
		/* File doesn't exist yet so we create it. */
		if (oflag & O_CREAT)
		{
			int ret = _creat_vfs(path, mode, mount_point);
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

	if ((fildes->status->st_mode & S_IFDIR) && (oflag & O_WRONLY))
	{
		errno = EISDIR;
		goto fail;
	}

	int fd;

	/* Use fs-provided open(), if available */
	if (fildes->dev->fs->open != NULL)
	{
		fd = fildes->dev->fs->open(reduced_path, oflag, mode, fildes);
	}
	else
	{
		fd = get_free_fd(3);
	}

	if (fd >= 0)
	{
		/* Populate the fildes */
		fildes->at = 0;
		fildes->oflag = oflag;
		fildes->sysflag |= FD_USED;

		memcpy(&(cur_task->files[fd]), fildes, sizeof(struct fildes_t));
		kfree(fildes);
		kfree(dir);

		return fd;
	}
	else
	{
		errno = EMFILE;
		goto fail;
	}

fail:
	if (fildes->status)
		kfree(fildes->status);
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

	if ((cur_task->files[fd].sysflag & FD_USED) == 0)
	{
		errno = EBADF;
		return -1;
	}

	if (cur_task->files[fd].sysflag & PIPE)
	{
		close_pipe(fd);
	}
	else if (cur_task->files[fd].dev->fs->close != NULL)
	{
		int ret = cur_task->files[fd].dev->fs->close(fd, cur_task->files[fd].path, cur_task->files[fd].dev);
		/* If this fails, errno should be set */
		if (ret < 0)
			return -1;
	}

	kfree(cur_task->files[fd].status);
	cur_task->files[fd].status = NULL;
	cur_task->files[fd].path = NULL;
	cur_task->files[fd].at = 0;
	cur_task->files[fd].oflag = 0;
	cur_task->files[fd].sysflag = 0;
	cur_task->files[fd].dev = NULL;

	return 0;
}

int _creat_vfs(const char* path, mode_t mode, struct mount_t* mount)
{
	if (path == NULL || mount == NULL)
	{
		errno = ENOENT;
		return -1;
	}

	struct fildes_t* fildes = kmalloc(sizeof(struct fildes_t));
	if (fildes == NULL)
		return -1;

	fildes->status = kmalloc(sizeof(struct stat));
	if (fildes->status == NULL)
	{
		kfree(fildes);
		return -1;
	}

	/* If access mode is unspecified, use 0-rwx-rwx-rwx */
	if ((mode & S_IPERM) == 0)
		mode |= (S_IRWXU | S_IRWXG | S_IRWXO);

	fildes->path = kmalloc(strlen(path)+1);
	if (fildes->path == NULL)
	{
		kfree(fildes->status);
		kfree(fildes);
		return -1;
	}

	strcpy(fildes->path, path);
	fildes->status->st_mode = mode;
	fildes->status->st_uid = 0;
	fildes->status->st_gid = 0;
	fildes->status->st_size = 0;
	fildes->status->st_dev = mount->device->devid;
	fildes->at = 0;
	fildes->oflag = O_WRONLY;
	fildes->sysflag = FD_USED;
	fildes->dev = mount->device;

	int ret = 0;

	if (mount->device->fs->creat != NULL)
	{
		ret = mount->device->fs->creat(reduce_path(path, mount->path), mode, mount->device, fildes);
		if (ret < 0)
		{
			kfree(fildes->status);
			kfree(fildes);
			return -1;
		}
	}

	/* Find an empty fildes from the task struct */

	int fd = get_free_fd(3);
	if (fd < 0 || fd > OPEN_MAX)
	{
		errno = EMFILE;
		kfree(fildes->status);
		kfree(fildes);
		return -1;
	}

	/* Now i is  the first free index. Populate the fildes */
	memcpy(&(cur_task->files[fd]), fildes, sizeof(struct fildes_t));

	kfree(fildes);

	return fd;
}

int creat_vfs(const char* path, mode_t mode)
{
	if (path == NULL)
	{
		errno = ENOENT;
		return -1;
	}

	/* Find the FS that the file is created to by searching mount points */

	struct mount_t* mount_point = mount_list;

	char* dir = kmalloc(strlen(path)+1);
	if (dir == NULL)
	{
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
			kfree(dir);
			return -1;
		}
	}

	kfree(dir);

mount_found:
	return _creat_vfs(path, mode, mount_point);
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
	if (cur_task->files[fd].status->st_mode & S_IFIFO)
	{
		errno = ESPIPE;
		return -1;
	}

	/* If there is some fs-special functionality, use that. It should
	 * set errno on errors.. */
	if (cur_task->files[fd].dev->fs->lseek != NULL)
		return cur_task->files[fd].dev->fs->lseek(fd, cur_task->files[fd].path, offset, whence, cur_task->files[fd].dev);

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
			cur_task->files[fd].at = cur_task->files[fd].status->st_size + offset;
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

/*int mount_vfs(char* src, char* dest, char* fs_type, uint32_t flags, void* data)
{
	* figure out dev from <src> and call dev_mount() *

	return 0;
}*/
