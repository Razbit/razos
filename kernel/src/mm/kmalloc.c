/* This file is a part of the RazOS project
 *
 * kmalloc.c -- kernel-world malloc() family
 *
 * Razbit 2015 */

#include <kmalloc.h>

#include <sys/types.h>
#include <asm/x86.h>
#include <string.h>
#include <errno.h>
#include <console.h>

#include "paging.h"
#include "kvm.h"

#include "../vga.h"

#define KMALLOC_FREE 1
#define KMALLOC_RES 0

static void* kheap_start = NULL;
static void* kheap_end = NULL;
static size_t kheap_size = 0;

struct kheap_node_t
{
	size_t size;
	uint32_t status; /* free or reserved */
	struct kheap_node_t* next;
	struct kheap_node_t* prev;
};

#define NODE_T_SIZE (sizeof(struct kheap_node_t)) /* 16 bytes */

static size_t round_up(size_t num, size_t align)
{
	if ((num & (align-1)) != 0)
	{
		num += align;
		num -= num % align;
	}

	return num;
}

static void* do_kmalloc(size_t size, size_t align)
{
	/* Start address is aligned to [align] that is a multiple of 16.
	 * End of the block is aligned so that the entire chunk's end is
	 * aligned to 16 bytes. */

	if (size == 0)
		return NULL;

	size = round_up(size, sizeof(struct kheap_node_t));

	/* Make sure we have a heap */
	if (kheap_start == NULL)
	{
		kheap_start = ksbrk(size + PAGE_SIZE);
		if (kheap_start == (void*)-1)
		{
			errno = ENOMEM;
			return NULL;
		}

		kheap_size = ksbrk(0) - kheap_start;
		struct kheap_node_t* start = (struct kheap_node_t*)kheap_start;
		start->size = kheap_size - sizeof(struct kheap_node_t);
		start->status = KMALLOC_FREE;
		start->next = NULL;
		start->prev = NULL;
	}

	struct kheap_node_t* startnode = (struct kheap_node_t*)kheap_start;
	struct kheap_node_t* curnode = startnode;

	void* ret = NULL;

	/* Find a decent node */
	while (curnode != NULL)
	{
		if (curnode->next == NULL)
		{
			/* Enlarge the heap. */
			if (curnode->size < size + PAGE_SIZE)
			{
				void* prev_end = ksbrk(size + PAGE_SIZE - curnode->size);
				void* new_end = ksbrk(0);
				if (prev_end == (void*)-1)
				{
					errno = ENOMEM;
					return NULL;
				}
				curnode->size += (size_t)(new_end - prev_end);
				kheap_size += (size_t)(new_end - prev_end);
			}
		}

		if (curnode->status == KMALLOC_RES)
		{
			curnode = curnode->next;
			continue;
		}

		size_t padding_amount = 0;
		if (((size_t)(curnode+sizeof(struct kheap_node_t)) % align) != 0)
		{
			padding_amount = align - ((size_t)curnode % align);
		}

		if (curnode->size < (size + padding_amount))
		{
			curnode = curnode->next;
			continue;
		}

		/* If we reach this, we have found a decent node */

		struct kheap_node_t* padding = curnode;
		if (padding_amount != 0 \
		    && padding_amount != sizeof(struct kheap_node_t))
		{
			/* Set the padding node */
			curnode = (void*)curnode + padding_amount \
				- sizeof(struct kheap_node_t);
			padding->size = padding_amount \
				- 2 * sizeof(struct kheap_node_t);
			padding->status = KMALLOC_FREE;
			curnode->next = padding->next;
			padding->next = curnode;
			curnode->prev = padding;
			curnode->size = (size_t)curnode->next - (size_t)curnode \
				- sizeof(struct kheap_node_t);
		}

		/* Pad if we need to */
		if (curnode->size != size)
		{
			padding = (void*)curnode + sizeof(struct kheap_node_t) + size;
			padding->prev = curnode;
			padding->next = curnode->next;
			if (padding->next != NULL)
			{
				padding->size = (size_t)padding->next - (size_t)padding \
					- sizeof(struct kheap_node_t);
				padding->next->prev = padding;
			}
			else
			{
				padding->size = ((size_t)kheap_start + kheap_size) \
					- (size_t)padding - sizeof(struct kheap_node_t);
			}
			padding->status = KMALLOC_FREE;
			curnode->next = padding;
		}

		curnode->size = size;
		curnode->status = KMALLOC_RES;

		ret = (void*)((void*)curnode + sizeof(struct kheap_node_t));
		break;
	}

	return ret;
}

/* Unify two nodes (coalesce) */
static void unify_fwd(struct kheap_node_t* ptr)
{
	ptr->size += ptr->next->size + NODE_T_SIZE;
	ptr->next = ptr->next->next;
	
	if (ptr->next != NULL)
		ptr->next->prev = ptr;
}

void do_kfree(void* addr)
{
	addr -= NODE_T_SIZE;
	struct kheap_node_t* ptr = addr;

	/* Mark as free */
	ptr->status = KMALLOC_FREE;

	/* Coalesce the surrounding free memory */
	if (ptr->next != NULL && ptr->next->status == KMALLOC_FREE)
		unify_fwd(ptr);
	if (ptr->prev != NULL && ptr->prev->status == KMALLOC_FREE)
	{
		ptr = ptr->prev;
		unify_fwd(ptr);
	}

	/* If this is the last node, free some of the heap */
	if (ptr->next == NULL && ptr->size > (2 * PAGE_SIZE))
	{
		ksbrk(-(ptr->size));
		kheap_end = ksbrk(0);
	}
}

/* Used when we don't yet have paging.. */
static void* pre_paging_kmalloc(size_t size, size_t align)
{
	void* cur_alloc_start = set_alloc_start(NULL);
	
	cur_alloc_start = (void*)round_up((size_t)cur_alloc_start, align);
	
	void* ret = cur_alloc_start;
	set_alloc_start(cur_alloc_start + size);
	
	return ret;	
}

/* Aligned */
void* kmalloc_a(size_t size, size_t align)
{
	align = round_up(align, NODE_T_SIZE);

	if (!check_cr0(CR0_PG))
		return pre_paging_kmalloc(size, align);
	else
		return do_kmalloc(size, align);
}

/* Page aligned */
void* kmalloc_pa(size_t size)
{
	return kmalloc_a(size, PAGE_SIZE);
}

/* Normal malloc() */
void* kmalloc(size_t size)
{
	return kmalloc_a(size, NODE_T_SIZE);
}

/* Normal free() */
void kfree(void* ptr)
{
	if (check_cr0(CR0_PG))
		do_kfree(ptr);
}

static int print_kheap_node_t(struct kheap_node_t* node)
{
	if (node->status == KMALLOC_FREE)
	{
		kprintf("%p\t%u\t%u\t\t%s\t%x\t%x\n", node, \
	        node->size, node->size + sizeof(struct kheap_node_t),	\
	        node->status == KMALLOC_FREE ? "free" : "used",		\
	        node->prev, node->next);
		return 1;
	}
	return 0;
}


void dump_kheap()
{
	kprintf("**KERNEL HEAP DUMP**\n");
	struct kheap_node_t* ptr = (struct kheap_node_t*)kheap_start;
	kprintf("KHEAP: SIZE: 0x%p START: 0x%p END: 0x%p\n", kheap_end - kheap_start, kheap_start, kheap_end);
	int i = 0;
	while (ptr->next != NULL && (void*)ptr < kheap_end)
	{
		ptr = ptr->next;

	}

	while (ptr != NULL)
	{
		if (i % 20 == 0)
			kprintf("NODE\t\tSIZE\tWITH NODE\tSTATUS\tPREV\tNEXT\n");
		i += print_kheap_node_t(ptr);
		ptr = ptr->prev;
	}
	kprintf("\n");
}
