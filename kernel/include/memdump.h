/* memdump.c -- UNIX hexdump -like utility */

/* Copyright (c) 2014, 2017 Eetu "Razbit" Pesonen
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

/* This version only prints the kind of output you get when
 * passing -C to the UNIX hexdump */

#ifndef MEMDUMP_H
#define MEMDUMP_H

#include <stdint.h>

void memdump(void* addr, size_t num);

#endif /* MEMDUMP_H */

