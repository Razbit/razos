/* This file is a part of the RazOS project
 *
 * fifofs.h -- fifo -files
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <kmalloc.h>
#include <errno.h>

#include "../mm/task.h"
#include "../mm/sched.h"

#include "vfs.h"
#include "fifofs.h"

struct fifofs_hdr_t* fifofs_nodes[] = {NULL};

static ssize_t read_fifofs(int fd, void* buf, size_t size)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	struct fifofs_hdr_t* hdr = fifofs_nodes[node->status.st_ino];
	struct fifofs_data_t* curnode = hdr->data;

	/* According to POSIX/Oracle reading from empty fifo: */
	while (node->status.st_size == 0)
	{
		/* Return 0 if write side is closed */
		if (hdr->writers == 0)
			return 0;

		if (cur_task->files[fd].oflag & O_NONBLOCK)
		{
			errno = EAGAIN;
			return -1;
		}

		/* If O_NONBLOCK is not set and the fifo is empty, block
		 * until size > 0 or there are no writers */
		sched_switch();
	}
	
	/* Data is splitted in 256 byte blocks */
	/* Offset in the first node */
	off_t offset = hdr->read_at % 0x100;

	/* How much we can read */
	if (size > (size_t)node->status.st_size)
		size = node->status.st_size;

	/* Read the data to buf */
	size_t read = 0;
	while (read < size)
	{
		for (; offset < 256; offset++)
		{
			/* Stop at EOF */
			if (read >= size)
				goto exit;
			
			*(uint8_t*)(buf+read) = curnode->data[offset];
			read++;
			hdr->read_at++;
			node->status.st_size--;
		}

		/* The whole node is read, free it and go on to the next node */
		struct fifofs_data_t* temp = curnode->next;
		kfree(curnode);
		curnode = temp;
		hdr->write_at -= 256;
		offset = 0;
	}

exit:
	return read;
}

static ssize_t write_fifofs(int fd, const void* buf, size_t size)
{
	struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	struct fifofs_hdr_t* hdr = fifofs_nodes[node->status.st_ino];
	struct fifofs_data_t* curnode = hdr->data;

	/* If there is no reader, according to Oracle we should
	 * set errno to EPIPE and return -1 */
	if (hdr->readers == 0)
	{
		errno = EPIPE;
		return -1;
	}
	
	/* Find the location where we can start writing */
	size_t start_node = hdr->write_at / 0xFF;
	size_t offset = hdr->write_at % 0x100;
	
	for (size_t i = 0; i < start_node; i++)
		curnode = curnode->next;

	/* Write */
	size_t written = 0;
	while (written < size)
	{
		for (; offset < 256; offset++)
		{
			curnode->data[offset] = *(uint8_t*)(buf+written);
			written++;
			node->status.st_size++;
			hdr->write_at++;
			if (written >= size)
				break;
		}

		/* Allocate new node when curnode is full */
		curnode->next = \
			(struct fifofs_data_t*)kmalloc(sizeof(struct fifofs_data_t));
		if (curnode->next == NULL)
		{
			if (written == 0) /* If we couldn't write anything, fail */
				return -1; /* kmalloc sets errno */
			else
				goto exit;
		}

		curnode = curnode->next;
		curnode->next = NULL;
		offset = 0;
	}

exit:
	return written;
}

static int open_fifofs(struct vfs_node_t* node, int oflag, mode_t mode)
{
	(void)mode;

	struct fifofs_hdr_t* hdr = fifofs_nodes[node->status.st_ino];

	/* POSIX leaves behavior undefined when using O_RDWR, so we treat
	 * it as O_RDONLY */
	if ((oflag & O_ACCMODE) == O_RDWR)
		oflag &= ~O_WRONLY; /* Remove writable flag */

	if (oflag & O_RDONLY) /* Opening the read end */
	{
		hdr->readers++;

		/* Do not block if we are in non-blocking mode */
		if (!(oflag & O_NONBLOCK))
		{
			/* Wait until there is a writer */
			while(hdr->writers < 1)
				sched_switch();
		}

		/* Now we have a writer so we can return */
		return get_free_fd(3);
	}
	else if (oflag & O_WRONLY) /* Opening the write end */
	{
		hdr->writers++;

		/* Wait until there is a reader */
		while(hdr->readers < 1)
		{
			/* Or fail if we are in non-blocking mode */
			if (oflag & O_NONBLOCK)
			{
				errno = ENXIO;
				return -1;
			}
			
			sched_switch();
		}

		/* Now we have a reader -> return */
		return get_free_fd(3);
	}
	else
		return -1;
}

static int close_fifofs(int fd)
{
	struct fifofs_hdr_t* hdr = \
		fifofs_nodes[cur_task->files[fd].vfs_node->status.st_ino];
	
	if (cur_task->files[fd].oflag & O_RDONLY) /* O_RDWR also handled */
	{
		hdr->readers--;
		if (hdr->readers < 0)
			hdr->readers = 0;
	}
	else if (cur_task->files[fd].oflag & O_WRONLY)
	{
		hdr->writers--;
		if (hdr->writers < 0)
			hdr->writers = 0;
	}
	
	return 0;
}

int creat_fifofs(struct vfs_node_t* node, mode_t mode)
{
	(void)mode;
	
	/* Creat is set in creat_vfs() */
	node->read = &read_fifofs;
	node->write = &write_fifofs;
	node->open = &open_fifofs;
	node->close = &close_fifofs;
	node->lseek = NULL;

	fifofs_nodes[node->status.st_ino] = \
		(struct fifofs_hdr_t*)kmalloc(sizeof(struct fifofs_hdr_t));
	if (fifofs_nodes[node->status.st_ino] == NULL)
		return -1; /* kmalloc sets errno */

	fifofs_nodes[node->status.st_ino]->data = \
		(struct fifofs_data_t*)kmalloc(sizeof(struct fifofs_data_t));
	if (fifofs_nodes[node->status.st_ino]->data == NULL)
	{
		kfree(fifofs_nodes[node->status.st_ino]);
		return -1; /* kmalloc sets errno */
	}

	fifofs_nodes[node->status.st_ino]->write_at = 0;
	fifofs_nodes[node->status.st_ino]->read_at = 0;

	memset(&(fifofs_nodes[node->status.st_ino]->data->data[0]), 0, 256);

	return 1;
}

/* Create a fifo file. Declared in sys/stat.h */
int mkfifo(const char* path, mode_t mode)
{
	int fd = creat_vfs(path, mode | S_IFIFO);
	if (fd < 0)
		return -1; /* creat sets errno */
	close_vfs(fd);
	return 0;
}
