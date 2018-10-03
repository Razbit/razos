/* fat.h -- the FAT filesystem */

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

#ifndef FAT_H
#define FAT_H

#include <sys/types.h>
#include "../device.h"


#define n_sectors(_BPB)	  \
	((_BPB->n_sects == 0) ? _BPB->n_sects_l : _BPB->n_sects)
#define fat_size(_BPB)	  \
	((_BPB->fat_size == 0) ? _BPB->ebpb.fat32.fat_size : _BPB->fat_size)
#define root_size(_BPB)	  \
	(((_BPB->n_dirs * 32) + (_BPB->sect_size - 1)) / _BPB->sect_size)
#define data_start(_BPB)	  \
	(_BPB->n_res + (_BPB->n_fats * _BPB->fat_size) + root_size(_BPB))
#define data_sectors(_BPB)	  \
	(n_sectors(_BPB) - (_BPB->n_res + (_BPB->n_fats * fat_size(_BPB)) \
	                    + root_size(_BPB)))
#define n_clusters(_BPB)	  \
	(data_sectors(_BPB) / _BPB->clust_size)
#define root_start(_BPB)	  \
	(data_start(_BPB) - root_size(_BPB))
#define cluster_start(_cluster, _BPB)	\
	(((cluster-2) * _BPB->clust_size) + data_start(_BPB))

#define FAT12 0x12
#define FAT16 0x16
#define FAT32 0x32

/* Dir flags */
#define FAT_RDONLY 0x01
#define FAT_HIDDEN 0x02
#define FAT_SYSTEM 0x04
#define FAT_VOL_LABEL 0x08
#define FAT_SUBDIR 0x10
#define FAT_DIRTY  0x20 /* archive */
#define FAT_DEVICE 0x40
#define FAT_LFN 0x0F


/* Volume Boot Record */
/* BIOS Param Block */
struct fat_bpb_t
{
	uint8_t jmp[3];      /* Opcodes to jump over the bpb */
	uint8_t oem_id[8];   /* DOS version, not used */
	uint16_t sect_size;  /* Bytes per sector */
	uint8_t clust_size;  /* Sectors per cluster */
	uint16_t n_res;      /* Number of reserved sectors, start of a FAT */
	uint8_t n_fats;      /* Number of FATs */
	uint16_t n_dirs;     /* Number of dir entries in root dir */
	uint16_t n_sects;    /* Number of sectors in volume (if 0, over 64k)*/
	uint8_t media_type;  /* 0xF0 for 1.44M, 0xF8 for HDD */
	uint16_t fat_size;   /* Sectors per FAT, only for FAT12/16 */
	uint16_t track_size; /* Sectors per track, useless */
	uint16_t n_heads;    /* Number of sides/heads, useless */
	uint32_t n_hidden;   /* Number of hidden sectors */
	uint32_t n_sects_l;  /* Number of sectors if over 64k */

	/* Extended BPB */
	union
	{
		/* FAT12/16 ebpb */
		struct
		{
			uint8_t drive_num; /* Drive number. Useless */
			uint8_t res;       /* Flags in NT */
			uint8_t signature; /* Must be 0x28 or 0x29 */
			uint32_t vol_id;   /* Volume ID */
			uint8_t label[11]; /* Volume label, padded with 0x20 */
			uint8_t fs_id[8];  /* FS identifier string, do not trust */
			uint8_t pad[28];   /* Padding */
		} fat16;

		/* FAT32 ebpb */
		struct
		{
			uint32_t fat_size; /* Sectors per FAT */
			uint16_t flags;    /* dunno */
			uint8_t ver_min;   /* Minor version */
			uint8_t ver_maj;   /* Major version */
			uint32_t root_loc; /* Cluster number of root dir */
			uint16_t fs_info;  /* Sector number of fs_info */
			uint16_t boot_bak; /* Sector number of backup boot sector */
			uint8_t res[12];   /* Reserved */
			uint8_t drive_num; /* Drive number. Useless */
			uint8_t res1;       /* Flags in NT */
			uint8_t signature; /* Must be 0x28 or 0x29 */
			uint32_t vol_id;   /* Volume ID */
			uint8_t label[11]; /* Volume label, padded with 0x20 */
			uint8_t fs_id[8];  /* FS identifier string, do not trust */
		} fat32;

	} ebpb;

	/* plus 420 bytes of boot code and a signature (0xAA55) at 0x1FE */
} __attribute__((packed));


/* FS Info, on FAT32 */
/* wtf Microsoft */
struct fat32_fs_info_t
{
	uint32_t sign1;   /* 0x52 52 61 41 */
	uint8_t res[480];
	uint32_t sign2;   /* 0x72 72 41 61 */
	uint32_t free;    /* Num of free clusters or FFFFFFFF if unknown */
	uint32_t last;    /* Number of last allocated cluster */
	uint8_t res2[12];
	uint32_t sign3;   /* 0x00 00 55 AA */
}__attribute__((__packed__));

/* FAT Directory entry (8.3) */
struct fat_dir_entry_t
{
	uint8_t name[8];      /* Byte 0: 0x00: last entry 0x05: marked for delete
	                       *         0x2E: dot entry  0xE5: avail/deleted */
	uint8_t ext[3];
	uint8_t attr;
	uint8_t res;
	uint8_t creat_ms;     /* Creation time, in 10ms resolution [0..200] */
	uint16_t creat_h:5;   /* [0..23] */
	uint16_t creat_m:6;   /* [0..59] */
	uint16_t creat_s:5;   /* [0..29] (seconds/2) */
	uint16_t creat_y:7;   /* [0..127] -> [1980..2107] */
	uint16_t creat_mon:4; /* [1..12] */
	uint16_t creat_d:5;   /* [1..31] */
	uint16_t acc_y:7;
	uint16_t acc_mon:4;
	uint16_t acc_d:5;
	uint16_t clust_h;     /* High 16 bits of cluster num */
	uint16_t mod_h:5;
	uint16_t mod_m:6;
	uint16_t mod_s:5;
	uint16_t mod_y:7;
	uint16_t mod_mon:4;
	uint16_t mod_d:5;
	uint16_t clust_l;     /* Low 16 bit of cluster num */
	uint32_t size;
}__attribute__((__packed__));

void dump_bpb(struct fat_bpb_t* data);
int fat_type(struct fat_bpb_t* bpb);
int read_sector(void* buf, size_t sector, struct device_t* dev);
int read_root_16(struct fat_dir_entry_t* buf, struct fat_bpb_t* bpb, struct device_t* dev);
int init_fat(struct device_t* dev);


#endif /* FAT_H */
