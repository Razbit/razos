/* Copyright (c) 2016 Eetu "Razbit" Pesonen
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
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <kmalloc.h>
#include <errno.h>
#include <console.h>

#include "vfs.h"
#include "../mm/task.h"
#include "../mm/sched.h"

#include "pipe.h"

/* TODO: fildes.status.st_size cannot be used as is */

struct pipe_hdr_t* pipes = NULL;
ino_t pipe_inodes = 0;

ssize_t read_pipe(int fd, void* buf, size_t size)
{
	struct pipe_hdr_t* hdr = pipes;

	while (hdr)
	{
		if (hdr->inode == cur_task->files[fd].status->st_ino)
			break;
		hdr = hdr->next;
	}

	if (hdr == NULL)
	{
		errno = EBADF;
		return -1;
	}

	struct pipe_data_t* curnode = hdr->data;

	/* According to POSIX/Oracle reading from empty pipe: */
	while (cur_task->files[fd].status->st_size == 0)
	{
		/* Return 0 if write side is closed */
		if (hdr->writers == 0)
		{
			return 0;
		}

		if (cur_task->files[fd].oflag & O_NONBLOCK)
		{
			errno = EAGAIN;
			return -1;
		}

		/* If O_NONBLOCK is not set and the pipe is empty, block
		 * until size > 0 or there are no writers */
		sched_switch();
	}

	/* Data is splitted in 256 byte blocks */
	/* Offset in the first node */
	off_t offset = hdr->read_at % 0x100;

	/* How much we can read */
	if (size > (size_t)cur_task->files[fd].status->st_size)
		size = cur_task->files[fd].status->st_size;

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
			cur_task->files[fd].status->st_size--;
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

ssize_t write_pipe(int fd, const void* buf, size_t size)
{
	struct pipe_hdr_t* hdr = pipes;

	while (hdr)
	{
		if (hdr->inode == cur_task->files[fd].status->st_ino)
			break;
		hdr = hdr->next;
	}

	if (hdr == NULL)
	{
		errno = EBADF;
		return -1;
	}

	struct pipe_data_t* curnode = hdr->data;

	/* If there is no reader, according to POSIX we should
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
			cur_task->files[fd].status->st_size++;
			hdr->write_at++;
			if (written >= size)
				break;
		}

		/* Allocate new node when curnode is full */
		curnode->next = \
			(struct pipe_data_t*)kmalloc(sizeof(struct pipe_data_t));
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

int close_pipe(int fd)
{
	struct pipe_hdr_t* hdr = pipes;

	while (hdr)
	{
		if (hdr->inode == cur_task->files[fd].status->st_ino)
			break;
		hdr = hdr->next;
	}

	if (hdr == NULL)
	{
		errno = EBADF;
		return -1;
	}

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
		/* No readers or writers left -> delete the pipe */
		struct pipe_data_t* ptr = hdr->data;
		struct pipe_data_t* ptr2 = NULL;

		while (ptr != NULL)
		{
			ptr2 = ptr;
			ptr = ptr->next;
			kfree(ptr2);
		}

		if (hdr == pipes)
		{
			pipes = hdr->next;
			kfree(hdr);
		}
		else
		{
			hdr = pipes->next;
			struct pipe_hdr_t* hdr_prev = pipes;

			while (hdr)
			{
				if (hdr->inode == cur_task->files[fd].status->st_ino)
					break;
				hdr_prev = hdr;
				hdr = hdr->next;
			}

			if (hdr)
			{
				hdr_prev->next = hdr->next;
				kfree(hdr);
			}
		}
	}

	return 0;
}


/* Create a pipe. fd[0] is the read end, fd[1] is the write end */
int creat_pipe(int fd[2])
{
	fd[0] = get_free_fd(3);
	if (fd[0] < 0)
	{
		errno = EMFILE;
		return -1;
	}

	if (cur_task->files[fd[0]].status == NULL)
	{
		cur_task->files[fd[0]].status = kmalloc(sizeof(struct stat));
		if (cur_task->files[fd[0]].status == NULL)
			return -1;
	}

	cur_task->files[fd[0]].status->st_mode = 0;
	cur_task->files[fd[0]].status->st_uid = 0;
	cur_task->files[fd[0]].status->st_gid = 0;
	cur_task->files[fd[0]].status->st_size = 0;
	cur_task->files[fd[0]].status->st_dev = DEVID_PIPE;
	cur_task->files[fd[0]].status->st_rdev = DEVID_PIPE;
	cur_task->files[fd[0]].status->st_ino = pipe_inodes;
	cur_task->files[fd[0]].at = 0;
	cur_task->files[fd[0]].oflag = O_RDONLY;
	cur_task->files[fd[0]].sysflag = PIPE | FD_USED;
	cur_task->files[fd[0]].dev = NULL;
	cur_task->files[fd[0]].path = NULL;

	fd[1] = get_free_fd(3);
	if (fd[1] < 0)
	{
		errno = EMFILE;
		return -1;
	}

	cur_task->files[fd[1]].status = cur_task->files[fd[0]].status;
	cur_task->files[fd[1]].at = 0;
	cur_task->files[fd[1]].oflag = O_WRONLY;
	cur_task->files[fd[1]].sysflag = PIPE | FD_USED;
	cur_task->files[fd[1]].dev = NULL;
	cur_task->files[fd[1]].path = NULL;

	struct pipe_hdr_t* cur;

	if (pipes == NULL)
	{
		pipes = kmalloc(sizeof(struct pipe_hdr_t));
		if (pipes == NULL)
		{
			goto fail;
		}

		pipes->data = kmalloc(sizeof(struct pipe_data_t));
		if (pipes->data == NULL)
		{
			kfree(pipes);
			pipes = NULL;
			goto fail;
		}
		cur = pipes;
	}
	else
	{
		cur = pipes;

		while(cur->next)
			cur = cur->next;

		cur->next = kmalloc(sizeof(struct pipe_hdr_t));
		if (cur->next == NULL)
		{
			goto fail;
		}

		cur->next->data = kmalloc(sizeof(struct pipe_data_t));
		if (cur->next->data == NULL)
		{
			kfree(cur->next);
			cur->next = NULL;
			goto fail;
		}
		cur = cur->next;
	}

    cur->write_at = 0;
	cur->read_at = 0;
	cur->writers = 2;
	cur->readers = 2;
	cur->inode = pipe_inodes;
	cur->next = NULL;

	pipe_inodes++;

	return 0;

fail:
	kfree(cur_task->files[fd[0]].status);
	memset(&(cur_task->files[fd[0]]), 0, sizeof(struct fildes_t));
	memset(&(cur_task->files[fd[1]]), 0, sizeof(struct fildes_t));
	return -1;
}
