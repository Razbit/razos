/* This file is a part of the RazOS project
 *
 * initrd.c -- initial ramdisk system
 *
 * Razbit 2014, 2015 */

#include <sys/types.h>
#include <string.h>
#include <console.h>
#include <kmalloc.h>
#include <fcntl.h>
#include <unistd.h>

#include "vfs.h"
#include "initrd.h"


/* Go through the initrd, create corresponding files to ramfs
 * and copy the data. */
void init_initrd(void* loc)
{
	uint32_t nfiles = *((uint32_t*)loc);
	struct initrd_node_t* node = (struct initrd_node_t*)(loc+4);

	for (uint32_t i = 0; i < nfiles; i++)
	{
		size_t size = node[i].size;
		off_t offset = node[i].offset + (off_t)loc;
		char* name = node[i].name;

		int fd = open_vfs(name, O_RDWR | O_CREAT, S_IFREG);
		write_vfs(fd, (void*)offset, size);
		close_vfs(fd);
	}
}
