/* This file is a part of the RazOS project
 *
 * stdout.c -- standard output
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <kmalloc.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <console.h> /* For now, a better terminal on the way */

#include "../mm/task.h"

#include "vfs.h"
#include "ramfs.h"

static ssize_t write_stdout(int fd, const void* buf, size_t size);

/* Initialize stdout. We use modified ramfs nodes in the background */
int init_stdout()
{
	int fd = open_vfs("stdout", O_RDWR | O_CREAT);

	/* Now that we have a file in ramfs, let's modify the vfs_node */
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;

	node->write = &write_stdout;
	node->read = NULL;

	return 1;
}

static ssize_t write_stdout(int fd, const void* buf, size_t size)
{
	size_t i = 0;
	for (; i < size; i++)
	{
		/* Write first */
		write_ramfs(fd, (char*)(buf+i), 1);
		
		/* Then check if newline -> flush */
		if (((char*)buf)[i] == '\n')
		{
			/* Stop writing to file, begin reading from it and write
			 * all of its contents to the screen */
			lseek_vfs(fd, 0, SEEK_SET);
			char c;
			int ret = 0;
			while (1)
			{
				ret = read_ramfs(fd, &c, 1);
				if (ret == 1)
					kputchar(c);
				else
					break;
			}
			/* Clean up the file */
			lseek_vfs(fd, 0, SEEK_SET);
			struct vfs_node_t* node = cur_task->files[fd].vfs_node;
			node->size = 0;

			/* Free the memory */
			struct ramfs_data_t* prevnode = \
				ramfs_nodes[node->inode - RAMFS_OFFSET];
			if (prevnode->next != NULL)
			{
				prevnode = prevnode->next;
				struct ramfs_data_t* curnode = prevnode->next;
				
				while (prevnode)
				{
					kfree(prevnode);
					prevnode = curnode;
					if (curnode)
						curnode = curnode->next;
				}
				ramfs_nodes[node->inode - RAMFS_OFFSET]->next = NULL;
			}
			/* Make sure the initial data node is full of NULL */
			memset(&(ramfs_nodes[node->inode - RAMFS_OFFSET]->data[0]), \
			       0, 256);
		}
	}
}
