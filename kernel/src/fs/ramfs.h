/* ramfs.h -- RAM File System */

/* Copyright (c) 2015 Eetu "Razbit" Pesonen
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
