/* vfs.h -- The kernel Virtual File System */

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

#ifndef VFS_H
#define VFS_H

#include <sys/types.h>
#include <sys/stat.h>
#include "device.h"

/* fildes_t sysflag values */
#define FD_USED 1

struct fildes_t
{
	char* path;
	struct stat status;
	off_t at;                    /* Where are we in the file */
	uint32_t oflag;
	uint32_t sysflag;
	struct device_t* dev;
};

struct fs_t
{
	ssize_t (*read)(size_t, char*, void*, size_t, struct device_t*);
	ssize_t (*write)(size_t, char*, const void*, size_t, struct device_t*);
	int (*close)(size_t, char*, struct device_t*);
	off_t (*lseek)(size_t, char*, off_t, int, struct device_t*);
	int (*open)(char*, int, mode_t, struct device_t*);
	int (*creat)(char*, mode_t, struct device_t*);
	int (*exist)(char*, struct device_t*);
};

/* Standard read, write, open, creat, close and lseek */
ssize_t read_vfs(int fd, void* buf, size_t size);
ssize_t write_vfs(int fd, const void* buf, size_t size);
int open_vfs(const char* path, int oflag, mode_t mode);
int creat_vfs(const char* path, mode_t mode);
int close_vfs(int fd);
off_t lseek_vfs(int fd, off_t offset, int whence);

int mount_vfs(char* src, char* dest, char* fs_type, uint32_t flags, void* data);

int do_fcntl(int fd, int cmd, uint32_t arg);

/* Get first free file descriptor starting from fd */
int get_free_fd(int fd);

#endif /* VFS_H */
