/* This file is a part of the RazOS project
 *
 * vfs.h -- The kernel Virtual File System
 *
 * Razbit 2014, 2015 */

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

#endif /* VFS_H */
