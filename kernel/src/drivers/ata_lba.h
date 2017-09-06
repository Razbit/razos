/* ata_lba.h -- ATA LBA-mode i/o */

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

#ifndef ATA_LBA_H
#define ATA_LBA_H

#include <sys/types.h>

/* uint32_t lba: bits 0..27: lba
 *               bit  28:    drive
 *               bits 29..31 reserved
 */

void ata_lba_read(uint32_t lba, uint8_t n_sect, void* buf);
void ata_lba_write(uint32_t lba, uint8_t n_sect, void* buf);

#endif /* ATA_LBA_H */
