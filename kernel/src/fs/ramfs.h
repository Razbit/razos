/* This file is a part of the RazOS project
 *
 * ramfs.h -- RAM File System, the (currently) only
 * filesystem implementation (not to be confused with initrd)
 *
 * Razbit 2015 */

#ifndef RAMFS_H
#define RAMFS_H

#include <sys/types.h>

/* Each of these data nodes contain 256 bytes of the file's data */
struct ramfs_data_t
{
	uint8_t data[256];
	struct ramfs_data_t* next;
};

/* Pointers to ramfs file data are stored here. VFS-supplied inode is
 * the index in the array */
extern struct ramfs_data_t* ramfs_nodes[1024];
extern uint32_t ramfs_inodes;

ssize_t read_ramfs(int, void*, size_t);
ssize_t write_ramfs(int, const void*, size_t);
int creat_ramfs(struct vfs_node_t*, uint32_t);

#endif /* RAMFS_H */
