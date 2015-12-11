/* This file is a part of the RazOS project
 *
 * initrd.h -- initial ramdisk system
 *
 * Razbit 2014, 2015 */

#ifndef INITRD_H
#define INITRD_H

#include <sys/types.h>

struct initrd_node_t
{
    char name[64];
    off_t offset; /* Offset from the beginning of initrd */
    size_t size;
};

/* Initialize the initrd */
void init_initrd(void* loc);

#endif /* INITRD_H */
