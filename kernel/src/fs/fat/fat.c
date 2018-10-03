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
#include "../device.h"

#include <sys/types.h>
#include <console.h>


int fat_type(struct fat_bpb_t* bpb)
{
	int ret;
	if (n_clusters(bpb) < 4085)
		ret = FAT12;
	else if (n_clusters(bpb) < 65525)
		ret = FAT16;
	else if (n_clusters(bpb) < 268435445)
		ret = FAT32;
	else
		ret = -1;

	return ret;
}

int read_root_16(struct fat_dir_entry_t* buf, struct fat_bpb_t* bpb, struct device_t* dev)
{
	int i, ret = -1;
	for (i = 0; i < root_size(bpb); i++)
	{
		ret = read_sector((buf + i * (bpb->sect_size / 32)), root_start(bpb)+i, dev);
		if (ret < 0)
			goto fail;
	}

	return i;

fail:
	return -1;

}

int read_sector(void* buf, size_t sector, struct device_t* dev)
{
	return dev->dev_read(buf, sector, 1, dev);
}

ssize_t fat_read(int fd, char* path, void* buf, size_t size, struct device_t* dev)
{
	return -1;
}

ssize_t fat_write(int fd, char* path, const void* buf, size_t size, struct device_t* dev)
{
	return -1;
}

int fat_close(int fd, char* path, struct device_t dev)
{
	return -1;
}

int fat_open(char* path, int oflag, mode_t mode, struct fildes_t* fildes)
{
	return -1;
}

int fat_creat(char* path, mode_t mode, struct device_t* dev, struct fildes_t* fildes)
{
	return -1;
}


int fat_exist(char* path, void* _fildes)
{
	/* populate _fildes->status */
	return 0;
}


int fat_mount(char* path, struct device_t* dev)
{
	/* copy FAT contents to a kernel buffer */
	return 0;
}

int fat_closedir(DIR* dir)
{
	return 0;
}

DIR* fat_opendir(const char* path)
{
	return NULL;
}

struct dirent* fat_readdir(DIR* dir)
{
	return NULL;
}




/* tell devfs this device has a fat fs on it */
int init_fat(struct device_t* dev)
{
	dev->fs = kmalloc(sizeof(struct fs_t));

	if (dev->fs == NULL)
		return -1;

	dev->fs->read = fat_read;
	dev->fs->write = fat_write;
	dev->fs->close = fat_close;
	dev->fs->open = fat_open;
	dev->fs->lseek = NULL;
	dev->fs->creat = fat_creat;
	dev->fs->exist = fat_exist;
	dev->fs->mount = fat_mount;
	dev->fs->opendir = fat_opendir;
	dev->fs->closedir = fat_closedir;
	dev->fs->readdir = fat_readdir;

	return 0;
}

/* Debugging functions */

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

void dump_bpb(struct fat_bpb_t* data)
{
	kprintf("FAT BPB dump\n");
	kprintf("OEM ID: %s\n", parse_fat_str(data->oem_id, 8));
	kprintf("Sector size: %d bytes\n", data->sect_size);
	kprintf("Cluster size: %d sectors\n", data->clust_size);
	kprintf("Reserved sectors: %d\n", data->n_res);
	kprintf("FATs: %d\n", data->n_fats);
	kprintf("Dirs: %d\n", data->n_dirs);
	kprintf("Sectors: %d (%d)\n", data->n_sects, data->n_sects_l);
	kprintf("Media: %x\n", data->media_type);
	kprintf("FAT size: %d\n", data->fat_size);
	kprintf("Track size: %d\n", data->track_size);
	kprintf("Heads: %d\n", data->n_heads);
	kprintf("Hidden sectors: %d\n", data->n_hidden);
	kprintf("FAT type: %x\n", fat_type(data));
}
