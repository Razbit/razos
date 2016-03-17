/* Copyright (c) 2014, 2015 Eetu "Razbit" Pesonen
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

#ifndef INITRD_H
#define INITRD_H

#include <sys/types.h>

struct initrd_node_t
{
    char name[64];
    off_t offset; /* Offset from the beginning of initrd */
    size_t size;
};

/* Initialize the initrd */
void init_initrd(void* loc);

#endif /* INITRD_H */
