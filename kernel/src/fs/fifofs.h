/* This file is a part of the RazOS project
 *
 * fifofs.h -- fifo -files
 *
 * Razbit 2016 */

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
