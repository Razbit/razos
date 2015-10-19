/* This file is a part of the RazOS project
 *
 * initrd.h -- initial ramdisk system
 *
 * Razbit 2014 */

#ifndef INITRD_H
#define INITRD_H

#include <sys/types.h>
#include "fs.h"

struct initrd_node_t
{
    char name[64];
    uint32_t offset; /* How far is the data from the beginning of initrd */
    size_t size;
};

struct fs_node_t* init_initrd(void* loc);

#endif /* INITRD_H */
