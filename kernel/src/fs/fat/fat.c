/* fat.c -- the FAT filesystem */

/* Copyright (c) 2017 Eetu "Razbit" Pesonen
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

/* No support for LFN mode */

#include "fat.h"

#include <sys/types.h>
#include <console.h>

void dump_bpb(struct fat_bpb_t* data)
{
	kprintf("FAT BPB dump\n");
	kprintf("OEM ID: %s\n", parse_fat_str(data->oem_id, 8));
	kprintf("Sector size: %d bytes\n", data->sect_size);
	kprintf("Cluster size: %d sectors\n", data->clust_size);
	kprintf("Reserved sectors: %d\n", data->res_sect);
	kprintf("FATs: %d\n", data->n_fats);
	kprintf("Dirs: %d\n", data->n_dirs);
	kprintf("Sectors: %d (%d)\n", data->n_sects, data->n_sects_l);
	kprintf("Media: %x\n", data->media_type);
	kprintf("FAT size: %d\n", data->fat_size);
	kprintf("Track size: %d\n", data->track_size);
	kprintf("Heads: %d\n", data->n_heads);
	kprintf("Hidden sectors: %d\n", data->n_hidden);
}

char* parse_fat_str(char* str, size_t len)
{
	if (len < 1)
		return str;

	len--;
	str[len] = 0;

	len--;

	for (; len >= 0; len--)
	{
		if (str[len] == ' ')
			str[len] = 0;
		else
			break;
	}

	return str;
}
