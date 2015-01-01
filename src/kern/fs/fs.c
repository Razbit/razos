/* This file is a part of the RazOS project
 *
 * fs.h -- The kernel Virtual File System
 *
 * Razbit 2014 */

#include "fs.h"

#include <sys/types.h>

/* Root of the filesystem */
struct fs_node_t* fs_root = NULL;

ssize_t read_fs(struct fs_node_t* node, void* buf, size_t size, off_t offset)
{
    if (node->read != NULL)
        return node->read(node, buf, size, offset);
    else
        return 0;
}

ssize_t write_fs(struct fs_node_t* node, void* buf, size_t size, off_t offset)
{
    if (node->write != NULL)
        return node->write(node, buf, size, offset);
    else
        return 0;
}

int open_fs(struct fs_node_t* node, int oflag)
{
    if (node->open != NULL)
        return node->open(node, oflag);
    else
        return -1;
}

int close_fs(struct fs_node_t* node)
{
    if (node->close != NULL)
        return node->close(node);
    else
        return -1;        
}

struct dirent* readdir_fs(struct fs_node_t* node)
{
    if ((node->flags & 0x07) == FS_DIR && node->readdir != NULL)
        return node->readdir(node);
    else
        return NULL;
}

struct fs_node_t* finddir_fs(struct fs_node_t* node, char* name)
{
    if ((node->flags & 0x07) == FS_DIR && node->finddir != NULL)
        return node->finddir(node, name);
    else
        return NULL;
}
