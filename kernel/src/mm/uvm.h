/* uvm.h -- user virtual memory */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
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

#ifndef UVM_H
#define UVM_H

#include <sys/types.h>

#include "paging.h"

/* Allocate a page for uvm use, return its address */
void* uvm_page_alloc();

/* Free the last page allocated to uvm */
void* uvm_page_free();

/* POSIX brk, sbrk */
int uvm_brk(void* addr);
void* uvm_sbrk(intptr_t incr);

#endif /* UVM_H */
