/* vfs.h -- The kernel Virtual File System */

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

#ifndef VFS_H
#define VFS_H

#include <sys/types.h>
#include <sys/stat.h>

/* Device IDs, for st_dev in stat structure */
#define DEVID_RAMFS 1
#define DEVID_PIPE  2
#define DEVID_STDIO 3

struct vfs_node_t;

typedef ssize_t (*read_t)(int, void*, size_t);
typedef ssize_t (*write_t)(int, const void*, size_t);
typedef int (*open_t)(struct vfs_node_t*, int, mode_t);
typedef int (*creat_t)(struct vfs_node_t*, mode_t);
typedef int (*close_t)(int);
typedef off_t (*lseek_t)(int, off_t, int);

struct vfs_node_t
{
	char name[64];
	struct stat status;

	read_t read;
	write_t write;
	open_t open;
	creat_t creat;
	close_t close;
	lseek_t lseek;

	struct vfs_node_t* next;
};

struct fildes_t
{
	struct vfs_node_t* vfs_node;
	off_t at;                    /* Where are we in the file */
	uint32_t oflag;
};

/* Root of the filesystem */
extern struct vfs_node_t* vfs_root;

/* Standard read, write, open, creat, close and lseek */
ssize_t read_vfs(int fd, void* buf, size_t size);
ssize_t write_vfs(int fd, const void* buf, size_t size);
int open_vfs(const char* name, int oflag, mode_t mode);
int creat_vfs(const char* name, mode_t mode);
int close_vfs(int fd);
off_t lseek_vfs(int fd, off_t offset, int whence);

int do_fcntl(int fd, int cmd, uint32_t arg);

/* Get first free file descriptor starting from fd */
int get_free_fd(int fd);

#endif /* VFS_H */
