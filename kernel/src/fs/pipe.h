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
