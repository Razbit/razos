/* ata.c -- ATA devices */

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

#include <sys/types.h>
#include <portio.h>
#include <kmalloc.h>
#include <string.h>

#include "../fs/device.h"
#include "ata.h"

int ata_read_sector(void* buf, uint32_t lba, struct device_t* dev)
{
	uint8_t ch = (((uint8_t)dev->private) & 0xF0) ? 0x170 : 0x1F0;
	uint8_t drive = 0xE0; /* Set LBA mode */
	drive |= (((uint8_t)dev->private) & 0x0F) << 4; /* master or slave */
	drive |= (lba >> 24 & 0x0F);

	outb(ch + ATA_PORT_DEVSEL, drive);
	outb(ch + ATA_PORT_SECC, 1);
	outb(ch + ATA_PORT_LBA0, (uint8_t)(lba & 0xFF));
	outb(ch + ATA_PORT_LBA1, (uint8_t)(lba >> 8 & 0xFF));
	outb(ch + ATA_PORT_LBA2, (uint8_t)(lba >> 16 & 0xFF));
	outb(ch + ATA_PORT_CMD, ATA_READ);

	/* Wait for the device */
	while (!(inb(ch + ATA_PORT_STAT) & ATA_DRQ));

	/* Read the data */
	for (int i = 0; i < 256; i++)
		*(uint16_t*)((uint16_t*)buf + i * 2) = inw(ch + ATA_PORT_DATA);

	return 1;
}

int ata_read(void* buf, off_t offset, size_t size, struct device_t* dev)
{
	int ret = 0;
	for (int i = 0; i < size; i++)
		ret += ata_read_sector(buf + i * 512, offset, dev);
	return ret;
}

int ata_write_sector(void* buf, uint32_t lba, struct device_t* dev)
{
	uint8_t ch = (((uint8_t)dev->private) & 0xF0) ? 0x170 : 0x1F0;
	uint8_t drive = 0xE0; /* Set LBA mode */
	drive |= (((uint8_t)dev->private) & 0x0F) << 4; /* master or slave */
	drive |= (lba >> 24 & 0x0F);

	outb(ch + ATA_PORT_DEVSEL, drive);
	outb(ch + ATA_PORT_SECC, 1);
	outb(ch + ATA_PORT_LBA0, (uint8_t)(lba & 0xFF));
	outb(ch + ATA_PORT_LBA1, (uint8_t)(lba >> 8 & 0xFF));
	outb(ch + ATA_PORT_LBA2, (uint8_t)(lba >> 16 & 0xFF));
	outb(ch + ATA_PORT_CMD, ATA_WRITE);

	/* Wait for the device */
	while (!(inb(ch + ATA_PORT_STAT) & ATA_DRQ));

	/* Write the data */
	for (int i = 0; i < 256; i++)
		outw(ch + ATA_PORT_DATA, *((uint16_t*)buf + i * 2));

	return 1;
}

int ata_write(void* buf, off_t offset, size_t size, struct device_t* dev)
{
	int ret = 0;
	for (int i = 0; i < size; i++)
		ret += ata_write_sector(buf + i * 512, offset, dev);
	return ret;
}

struct device_t* ata_init()
{
	struct device_t* dev = kmalloc(sizeof(struct device_t));
	strcpy(dev->name, "sda");
	dev->devid = 0x00100000;
	dev->type = DEV_BLOCK;
	dev->fs == NULL;
	dev->dev_read = ata_read;
	dev->dev_write = ata_write;
	dev->private = ATA_PRI | ATA_MASTER;

	return dev;
}
