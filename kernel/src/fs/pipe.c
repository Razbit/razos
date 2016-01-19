/* This file is a part of the RazOS project
 *
 * pipe.c -- pipe functionality
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <kmalloc.h>

#include "vfs.h"
#include "../mm/task.h"
#include "../mm/sched.h"

#include "pipe.h"

struct pipe_hdr_t* pipes[] = {NULL};
ino_t pipe_inodes = 0;

static ssize_t read_pipe(int fd, void* buf, size_t size)
{
	if (size == 0)
		return 0;
	if (buf == NULL)
		return -1;

	struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	
	if (node == NULL)
	{
		/* TODO: set errno to EFAULT */
		return -1;
	}

	struct pipe_hdr_t* hdr = pipes[node->status.st_ino];
	struct pipe_data_t* curnode = hdr->data;

	/* According to POSIX/Oracle reading from empty pipe: */
	while (node->status.st_size == 0)
	{
		/* Return 0 if write side is closed */
		if (hdr->writers == 0)
			return 0;

		/* Return -1 and set errno to EAGAIN if O_NONBLOCK is set */
		if (cur_task->files[fd].oflag & O_NONBLOCK)
		{
			/* TODO: set errno to EAGAIN */
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
		struct pipe_data_t* temp = curnode->next;
		kfree(curnode);
		curnode = temp;
		hdr->write_at -= 256;
		offset = 0;
	}

exit:
	return read;
}

static ssize_t write_pipe(int fd, const void* buf, size_t size)
{
	if (size == 0)
		return 0;
	if (buf == NULL)
		return -1;

	struct vfs_node_t* node = cur_task->files[fd].vfs_node;
	
	if (node == NULL)
	{
		/* TODO: set errno to EFAULT */
		return -1;
	}

	struct pipe_hdr_t* hdr = pipes[node->status.st_ino];
	struct pipe_data_t* curnode = hdr->data;

	/* If there is no reader, according to Oracle we should
	 * set errno to EPIPE and return -1 */
	if (hdr->readers == 0)
	{
		/* TODO: set errno to EPIPE */
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
			(struct pipe_data_t*)kmalloc(sizeof(struct pipe_data_t));
		if (curnode->next == NULL)
			goto exit;
		curnode = curnode->next;
		curnode->next = NULL;
		offset = 0;
	}

exit:
	return written;
}

static int close_pipe(int fd)
{
	struct pipe_hdr_t* hdr = \
		pipes[cur_task->files[fd].vfs_node->status.st_ino];
	
	if (cur_task->files[fd].oflag & O_RDONLY)
	{
		/* Closing read-side */
		hdr->readers--;
	}
	else
	{
		/* Closing write-side */
		hdr->writers--;
	}

	if (hdr->readers == 0 && hdr->writers == 0)
	{
		/* No readers and writers left -> delete the pipe */
		struct pipe_data_t* ptr = hdr->data;
		struct pipe_data_t* ptr2 = NULL;
		while (ptr != NULL)
		{
			ptr2 = ptr;
			ptr = ptr->next;
			kfree(ptr2);			
		}
		kfree(hdr);
		pipes[cur_task->files[fd].vfs_node->status.st_ino] = NULL;
	}

	return 0;
}


/* Create a pipe. fd[0] is the read end, fd[1] is the write end */
int creat_pipe(int fd[2])
{
	if (pipe_inodes > 1023)
	{
		/* TODO: set errno to ENFILE */
		return -1;
	}
	
	struct vfs_node_t* node = vfs_root;
	struct vfs_node_t* node_backup = NULL;

	/* If this is the first file */
	if (node == NULL)
	{
		node = (struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		if (node == NULL)
			return -1;

		vfs_root = node;
	}
	else
	{
		/* Find the end of vfs */
		while (node->next != NULL)
			node = node->next;

		node->next = \
			(struct vfs_node_t*)kmalloc(sizeof(struct vfs_node_t));
		if (node->next == NULL)
			return -1;

		node_backup = node;
		node = node->next;
	}

	node->status.st_mode = 0;
	node->status.st_uid = 0;
	node->status.st_gid = 0;
	node->status.st_size = 0;
	node->next = NULL;
	node->name[0] = '\0';

	node->status.st_dev = DEVID_PIPE;
	node->status.st_rdev = DEVID_PIPE;
	node->status.st_ino = pipe_inodes++;
	node->close = &close_pipe;
	node->read = &read_pipe;
	node->write = &write_pipe;
	node->creat = NULL;
	node->open = NULL;
	node->lseek = NULL;

	fd[0] = get_free_fd(3);
	if (fd[0] < 0)
	{
		kfree(node_backup->next);
		node_backup->next = NULL;
		
		/* TODO: set errno to EMFILE */
		return -1;
	}

	cur_task->files[fd[0]].vfs_node = node;
	cur_task->files[fd[0]].at = 0;
	cur_task->files[fd[0]].oflag = O_RDONLY;
	
	fd[1] = get_free_fd(3);
	if (fd[1] < 0)
	{
		kfree(node_backup->next);
		node_backup->next = NULL;
		
		/* TODO: set errno to EMFILE */
		return -1;
	}

	cur_task->files[fd[1]].vfs_node = node;
	cur_task->files[fd[1]].at = 0;
	cur_task->files[fd[1]].oflag = O_WRONLY;

	pipes[node->status.st_ino] = \
		(struct pipe_hdr_t*)kmalloc(sizeof(struct pipe_hdr_t));
	if (pipes[node->status.st_ino] == NULL)
	{
		return -1;
	}
	pipes[node->status.st_ino]->write_at = 0;
	pipes[node->status.st_ino]->read_at = 0;	
	pipes[node->status.st_ino]->writers = 2;
	pipes[node->status.st_ino]->readers = 2;
	pipes[node->status.st_ino]->data = \
		(struct pipe_data_t*)kmalloc(sizeof(struct pipe_data_t));

	return 0;
}
