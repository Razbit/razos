/* kmalloc.h -- defines kernel-world malloc() family */

/* Copyright (c) 2014, 2016 Eetu "Razbit" Pesonen
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

#ifndef KMALLOC_H
#define KMALLOC_H

#include <sys/types.h>

/* Page aligned */
void* kmalloc_pa(size_t size);

/* Aligned to <align> bytes (multiple of 16)*/
void* kmalloc_a(size_t size, size_t align);

/* Normal malloc(). Aligned to 16 bytes */
void* kmalloc(size_t size);

/* Normal free() */
void kfree(void* ptr);

/* Dump kernel heap structures for debugging */
void dump_kheap();

#endif /* KMALLOC_H */
