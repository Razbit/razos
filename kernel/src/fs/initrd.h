/* This file is a part of the RazOS project
 *
 * initrd.h -- initial ramdisk system
 *
 * Razbit 2014 */

/* This is to be rethought.. */

#ifndef INITRD_H
#define INITRD_H

#include <sys/types.h>
#include "vfs.h"

struct initrd_node_t
{
    char name[64];
    uint32_t offset; /* Offset from the beginning of initrd */
    size_t size;
};

/* Initialize the initrd */
struct vfs_node_t* init_initrd(void* loc);

#endif /* INITRD_H */
