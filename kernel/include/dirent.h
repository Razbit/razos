/* This file is a part of the RazOS project
 *
 * dirent.h -- dirent structure
 *
 * Razbit 2014 */

#ifndef DIRENT_H
#define DIRENT_H

#include <sys/types.h>
#include "../kern/fs/fs.h"

#define DIR struct fs_node_t

struct dirent
{
	ino_t d_ino;   /* Inode */
	char d_name[]; /* Name of dir entry */
};

#endif /* DIRENT_H */
