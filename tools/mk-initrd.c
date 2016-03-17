/* mk-initrd.c -- a tool for creating the initrd image */

/* Copyright (c) 2014 Eetu "Razbit" Pesonen
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

/* We receive paths to files as arguments and we write those
 * files into the initrd.img */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct initrd_node_t
{
	char name[64];
	uint32_t offset;
	uint32_t size;
};

int main(int argc, char** argv)
{
	uint32_t nnodes = argc-1;
	if (nnodes < 1)
	{
		fprintf(stderr, "Usage: %s file1 [file2 ... filen]\n", argv[0]);
		return -1;
	}

	printf("%d file(s) are going to be written\n", nnodes);

	uint32_t offset = nnodes * sizeof(struct initrd_node_t);
	offset += sizeof(uint32_t);

	struct initrd_node_t nodes[nnodes];

	int i;
	for (i = 0; i < nnodes; i++)
	{
		printf("Writing file %s at 0x%X\n", argv[i+1], offset);
		strncpy(nodes[i].name, argv[i+1], 63);
		nodes[i].offset = offset;

		FILE* infile = fopen(argv[i+1], "r");
		if (infile == NULL)
		{
			fprintf(stderr, "File not found: %s\n", argv[i+1]);
			return -1;
		}

		fseek(infile, 0, SEEK_END);
		nodes[i].size = ftell(infile);
		offset += nodes[i].size;

		fclose(infile);

		printf("File \"%s\" is at 0x%X and is %d bytes long\n", \
		       nodes[i].name, nodes[i].offset, nodes[i].size);
	}

	/* Write the nodes to the image file */
	FILE* wfile = fopen("initrd.img", "w");

	fwrite(&nnodes, sizeof(nnodes), 1, wfile);
	fwrite(nodes, sizeof(struct initrd_node_t), nnodes, wfile);

	/* Write the actual contents of the files */
	for (i = 0; i < nnodes; i++)
	{
		uint8_t* buf = malloc(nodes[i].size);

		FILE* infile = fopen(argv[i+1], "r");
		fread(buf, 1, nodes[i].size, infile);
		fwrite(buf, 1, nodes[i].size, wfile);
		fclose(infile);

		free(buf);
	}

	fclose(wfile);

	return 0;
}
