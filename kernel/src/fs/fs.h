/* This file is a part of the RazOS project
 *
 * fs.h -- The kernel Virtual File System
 *
 * Razbit 2014 */

#ifndef FS_H
#define FS_H

#include <sys/types.h>
#include <dirent.h>

#define FS_FILE 0x01
#define FS_DIR 0x02
#define FS_CHAR 0x03
#define FS_BLOCK 0x04
#define FS_PIPE 0x05
#define FS_SYMLINK 0x06
#define FS_MNTPOINT 0x08 /* Not 0x07, now we can OR it with FS_DIR */

struct fs_node_t;
uint32_t inodes;

typedef ssize_t (*read_t)(struct fs_node_t*, void*, size_t, off_t);
typedef ssize_t (*write_t)(struct fs_node_t*, const void*, size_t, off_t);
typedef int (*open_t)(struct fs_node_t*, int oflag);
typedef int (*close_t)(struct fs_node_t*);
typedef struct dirent* (*readdir_t)(struct fs_node_t*);
typedef struct fs_node_t* (*finddir_t)(struct fs_node_t*, char*);

struct fs_node_t
{
    char name[128];           /* filename */
    mode_t mode;              /* permission mask */
    uid_t uid;                /* owning user */
    gid_t gid;                /* owning group */
    uint32_t flags;           /* node type */
    ino_t inode;              /* a way for the fs to identify files */
    off_t size;               /* size of the file */

    read_t read;
    write_t write;
    open_t open;
    close_t close;
    readdir_t readdir;
    finddir_t finddir;

    struct fs_node_t* ptr;    /* symlinks (and mounting) */
};

/* Root of the filesystem */
extern struct fs_node_t* fs_root;

/* Pretty much standard read, write, open and close, these deal with
 * fs_nodes instead of file descriptors though */
ssize_t read_fs(struct fs_node_t* node, void* buf, size_t size, off_t offset);
ssize_t write_fs(struct fs_node_t* node, void* buf, size_t size, off_t offset);
int open_fs(struct fs_node_t* node, int oflag);
int close_fs(struct fs_node_t* node);

struct dirent* readdir_fs(struct fs_node_t* node);
struct fs_node_t* finddir_fs(struct fs_node_t* node, char* name);

#endif /* FS_H */
