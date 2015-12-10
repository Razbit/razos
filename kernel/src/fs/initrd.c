/* This file is a part of the RazOS project
 *
 * initrd.c -- initial ramdisk system
 *
 * Razbit 2014 */

/* This is to be rethought.. */

#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <console.h>
#include <kmalloc.h>

#include "vfs.h"
#include "initrd.h"

static uint32_t initrd_nfiles;
static struct initrd_node_t* initrd_files;

static struct vfs_node_t* initrd_root;
static struct vfs_node_t* root_nodes;

static uint32_t nroot_nodes;

static struct vfs_node_t* prevnode = NULL;
static uint32_t index = 0;

static struct dirent dirent;
extern uint32_t inodes;

static ssize_t initrd_read(struct vfs_node_t* node, void* buf, \
	                           size_t size, off_t offset)
{
	kprintf("initrd_files: 0x%p\n", initrd_files);
	
    struct initrd_node_t file = initrd_files[node->inode-1];
    
    /* Are we trying to read from outside of the file */
    if ((size_t)offset > file.size)
        return 0;
    if ((size_t)((size_t)offset + size) > file.size)
        size = file.size - offset;
    memcpy(buf, (void*)(file.offset + offset), size);

    return size;
}

static struct dirent* initrd_readdir(struct vfs_node_t* node)
{
    if (prevnode != node)
    {
        index = 0;
        prevnode = node;
    }
    if (index >= nroot_nodes)
        return NULL;

    strcpy(dirent.d_name, root_nodes[index].name);
    dirent.d_name[strlen(root_nodes[index].name)] = '\0';
    dirent.d_ino = root_nodes[index].inode;

    index++;

    return &dirent;
}

static struct vfs_node_t* initrd_finddir(struct vfs_node_t* node, \
	                                         char* name)
{
	(void)node;
    uint32_t i;
    for (i = 0; i < nroot_nodes; i++)
        if (strcmp(name, root_nodes[i].name) == 0)
            return &root_nodes[i];

    return NULL;
}

/* Sets up the initrd file system for use by kernel */
struct vfs_node_t* init_initrd(void* loc)
{
	kprintf("loc: 0x%p\n", loc);

    initrd_nfiles = *((uint32_t*)loc);
    initrd_files = (struct initrd_node_t*)(loc + 4);

    /* set up the root directory */
    initrd_root = kmalloc(sizeof(struct vfs_node_t));

    strcpy(initrd_root->name, "initrd");
    initrd_root->mode = 0;
    initrd_root->uid = 0;
    initrd_root->gid = 0;
    initrd_root->inode = 0;
    initrd_root->size = 0;
    initrd_root->flags = VFS_DIR;
    initrd_root->read = NULL;
    initrd_root->write = NULL;
    initrd_root->open = NULL;
    initrd_root->close = NULL;
    initrd_root->readdir = &initrd_readdir;
    initrd_root->finddir = &initrd_finddir;
    initrd_root->ptr = NULL;

	inodes += 1;

    /* allocate space for the files in the ramdisk */
    root_nodes = kmalloc(sizeof(struct vfs_node_t) * initrd_nfiles);
    nroot_nodes = initrd_nfiles;

    uint32_t i;
    for (i = 0; i < initrd_nfiles; i++)
    {
        /* File's header now contains offset relative to the beginning
         * of the initrd. We want it to relative to start of memory */
	    initrd_files[i].offset += (off_t)loc;

        /* Create new file node for files in the initrd */
	    strncpy(root_nodes[i].name, initrd_files[i].name, 64);
        root_nodes[i].mode =0;
        root_nodes[i].uid = 0;
        root_nodes[i].gid = 0;
        root_nodes[i].size = initrd_files[i].size;
        root_nodes[i].inode = i+1;
        root_nodes[i].flags = VFS_FILE;
        root_nodes[i].read = &initrd_read;
        root_nodes[i].write = NULL;
        root_nodes[i].readdir = NULL;
        root_nodes[i].finddir = NULL;
        root_nodes[i].open = NULL;
        root_nodes[i].close = NULL;
    }

	inodes += i;

    return initrd_root;
}
