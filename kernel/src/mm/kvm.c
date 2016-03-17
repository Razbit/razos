/* kvm.c -- kernel virtual memory */

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

#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "kvm.h"
#include "paging.h"

static void* kheap_start = 0;
static size_t kheap_size = 0;

/* Initialize kvm, set beginning of kheap */
void kvm_init(void* start)
{
	kheap_start = start;
}

/* Allocate a page for kvm use */
void* kvm_page_alloc()
{
	void* ret = page_map((size_t)kheap_start + kheap_size, \
	                     frame_alloc(),	PF_PRES | PF_RW, cur_page_dir);
	if (ret == NULL)
	{
		errno = ENOMEM;
		return NULL;
	}

	kheap_size += PAGE_SIZE;

	return ret;
}

/* Allocate a page for kvm use, clear contents */
void* kvm_page_alloc_zeroed()
{
	void* ret = kvm_page_alloc();

	if (ret == NULL)
		return NULL;

	memset(ret, 0, PAGE_SIZE);
	return ret;
}

/* Free a kvm page */
void kvm_page_free()
{
	uint32_t addr = (size_t)kheap_start + kheap_size;
	frame_free(get_phys(addr, cur_page_dir));
	page_map(addr, 0, 0, cur_page_dir);

	kheap_size -= PAGE_SIZE;
}

/* Kernel's brk(). Set the end of heap to addr */
int kbrk(void* addr)
{
	if (addr < kheap_start)
		goto bad;
	if (addr > (void*)KSTACK_BEGIN)
		goto bad;

	if (addr == (void*)((size_t)kheap_start + kheap_size))
	{
		return 0;
	}
	else if (addr > (void*)((size_t)kheap_start + kheap_size))
	{
		/* Enlarge the heap */
		while (addr > (void*)((size_t)kheap_start + kheap_size))
		{
			if (kvm_page_alloc_zeroed() == NULL)
				goto bad;
		}
		return 0;
	}
	else
	{
		/* Shrink the heap */
		while (addr < (void*)((size_t)kheap_start + kheap_size))
			kvm_page_free();
		return 0;
	}

bad:
	errno = ENOMEM;
	return -1;
}

/* Kernel's sbrk(). Enlarge/shrink the heap by incr bytes */
void* ksbrk(intptr_t incr)
{
	void* ret = (void*)((size_t)kheap_start + kheap_size);

	if (incr > 0)
	{
		/* Enlarge */
		if (((size_t)kheap_start + kheap_size) + incr > KSTACK_BEGIN)
			goto bad;

		while (incr > 0)
		{
			if (kvm_page_alloc_zeroed() == NULL)
				goto bad;
			incr -= PAGE_SIZE;
		}
	}
	else if (incr < 0)
	{
		/* Shrink */
		if ((size_t)kheap_start+kheap_size+incr < (size_t)kheap_start)
			goto bad;

		incr += PAGE_SIZE - 1;
		while (incr < 0)
		{
			kvm_page_free();
			incr += PAGE_SIZE;
		}
	}

	return ret;

bad:
	errno = ENOMEM;
	return (void*)-1;
}
