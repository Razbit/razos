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

#ifndef FIFOFS_H
#define FIFOFS_H

#include <sys/types.h>
#include "vfs.h"

/* One node for 256 bytes of data */
struct fifofs_data_t
{
	uint8_t data[256];
	struct fifofs_data_t* next;
};

/* Header for fifo files */
struct fifofs_hdr_t
{
	struct fifofs_data_t* data;
	size_t write_at;
	size_t read_at;
	int readers;
	int writers;
};

/* List of fifo files */
extern struct fifofs_hdr_t* fifofs_nodes[1024];

int creat_fifofs(struct vfs_node_t*, mode_t);

#endif /* FIFOFS_H */
