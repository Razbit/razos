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

/* Volume Boot Record */
/* BIOS Param Block */
struct fat_bpb_t
{
	uint8_t jmp[3];      /* Opcodes to jump over the bpb */
	uint8_t oem_id[8];   /* DOS version, not used */
	uint16_t sect_size;  /* Bytes per sector */
	uint8_t clust_size;  /* Sectors per cluster */
	uint16_t res_sect;   /* Number of reserved sectors */
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
};

void dump_bpb(struct fat_bpb_t* data);
char* parse_fat_str(char* str, size_t len); /* remove padding, add null */

#endif /* FAT_H */
