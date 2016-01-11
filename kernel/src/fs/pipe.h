/* This file is a part of the RazOS project
 *
 * pipe.h -- pipe functionality
 *
 * Razbit 2016 */

#ifndef PIPE_H
#define PIPE_H

#include <sys/types.h>
#include "vfs.h"

/* Pipe's data splitted to 256 byte segments */
struct pipe_data_t
{
	uint8_t data[256];
	struct pipe_data_t* next;
};

struct pipe_hdr_t
{
	struct pipe_data_t* data;
	size_t write_at;
	size_t read_at;
	size_t writers;
    size_t readers;
};

extern struct pipe_hdr_t* pipes[1024];
extern ino_t pipe_inodes;

int creat_pipe(int fd[2]);

#endif /* PIPE_H */
