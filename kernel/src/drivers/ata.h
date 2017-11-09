/* ata.h -- ATA devices */

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

#ifndef ATA_H
#define ATA_H

#include <stdint.h>

/* Status register */
#define ATA_BSY  0x80  /* busy */
#define ATA_DRDY 0x40  /* dev ready */
#define ATA_DF   0x20  /* dev fault */
#define ATA_DSC  0x10  /* seek complete */
#define ATA_DRQ  0x08  /* data transfer requested */
#define ATA_CORR 0x04  /* data corrected */
#define ATA_IDX  0x02  /* index mark */
#define ATA_ERR  0x01  /* error */

/* Error register */
#define ATA_BBK   0x80 /* bad block */
#define ATA_UNC   0x40 /* uncorrectable data error */
#define ATA_MC    0x20 /* media changed */
#define ATA_IDNF  0x10 /* ID mark not found */
#define ATA_MCR   0x08 /* media change requested */
#define ATA_ABRT  0x04 /* cmd abort */
#define ATA_TK0NF 0x02 /* track 0 not found */
#define ATA_AMNF  0x01 /* addr mark not found */

/* Commands */
#define ATA_READ        0x20
#define ATA_WRITE       0x30
#define ATA_FLUSH       0xE7
#define ATA_FLUSH_EXT   0xEA
#define ATA_PACKET      0xA0
#define ATA_IDFY_PACKET 0xA1
#define ATA_IDENTIFY    0xEC

#define ATA_MASTER      0x00
#define ATA_SLAVE       0x01
#define ATA_PRI         0x00
#define ATA_SEC         0x10

/* Ports */
#define ATA_PORT_DATA   0x00
#define ATA_PORT_ERR    0x01
#define ATA_PORT_SECC   0x02
#define ATA_PORT_LBA0   0x03
#define ATA_PORT_LBA1   0x04
#define ATA_PORT_LBA2   0x05
#define ATA_PORT_DEVSEL 0x06
#define ATA_PORT_CMD    0x07
#define ATA_PORT_STAT   0x07

/* Channels */
#define ATA_CH_PRI      0x1F0
#define ATA_CH_SEC      0x170

struct device_t* ata_init();
int ata_read(void* buf, off_t offset, size_t size, struct device_t* dev);
int ata_write(void* buf, off_t offset, size_t size, struct device_t* dev);

#endif /* ATA_H */
