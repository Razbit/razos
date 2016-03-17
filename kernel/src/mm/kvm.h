/* kvm.h -- kernel virtual memory */

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

#ifndef KVM_H
#define KVM_H

#include "paging.h"

/* Initialize kvm, set beginning of kheap */
void kvm_init(void* start);

/* Allocate a page for kvm use */
void* kvm_page_alloc();
void* kvm_page_alloc_zeroed();

/* Free a kvm page */
void kvm_page_free();

/* Kernel's brk(). Set the end of heap to addr */
int kbrk(void* addr);

/* Kernel's sbrk(). Enlarge/shrink the heap by incr bytes */
void* ksbrk(intptr_t incr);

#endif /* KVM_H */
