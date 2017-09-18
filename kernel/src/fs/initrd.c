/* Copyright (c) 2014, 2015 Eetu "Razbit" Pesonen
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
	kprintf("Loading initrd\n");
	uint32_t nfiles = *((uint32_t*)loc);
	struct initrd_node_t* node = (struct initrd_node_t*)(loc+4);

	const char* prefix = "/rd/";

	for (uint32_t i = 0; i < nfiles; i++)
	{
		size_t size = node[i].size;
		off_t offset = node[i].offset + (off_t)loc;

		char* path = kmalloc(strlen(prefix) + strlen(node[i].name));
		if (path == NULL)
			break;

		strcat(path, prefix);
		strcat(path, node[i].name);

		int fd = open_vfs(path, O_RDWR | O_CREAT, S_IFREG);
		write_vfs(fd, (void*)offset, size);
		close_vfs(fd);
		kfree(path);
	}
}
