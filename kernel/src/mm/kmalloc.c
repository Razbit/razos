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

#define KMALLOC_FREE 1
#define KMALLOC_RES 0

static void* kheap_start = NULL;
static void* kheap_end = NULL;

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
	/* Start address aligned to [align] bytes.
	 * End of the chunk is at a 16-byte boundary. */

	if (size == 0)
		return NULL;

	size = round_up(size, NODE_T_SIZE);

	/* Do we have a heap? */
	if (kheap_start == NULL)
	{
		kheap_start = ksbrk(size + PAGE_SIZE);
		if (kheap_start == (void*)-1)
		{
			errno = ENOMEM;
			return NULL;
		}

		kheap_end = ksbrk(0);

		struct kheap_node_t* start = (struct kheap_node_t*)kheap_start;
		start->size = (size_t)(kheap_end - kheap_start);
		start->status = KMALLOC_FREE;
		start->next = NULL;
		start->prev = NULL;
	}

	struct kheap_node_t* cur_node = (struct kheap_node_t*)kheap_start;
	struct kheap_node_t* best_node = NULL;

	/* Find a decent node (best-fit strategy) */
	while (cur_node != NULL)
	{
		/* We are at the end of the list */
		if (cur_node->next == NULL)
		{
			/* We have found large-enough free node */
			if (best_node != NULL)
				break;
			
			/* The last node is usable but too small */
			if ((cur_node->size < size + PAGE_SIZE)
			    && (cur_node->status == KMALLOC_FREE))
			{
				/* Enlarge the heap */
				if (ksbrk(size+PAGE_SIZE-cur_node->size) == (void*)-1)
				{
					errno = ENOMEM;
					return NULL;
				}
				cur_node->size += (size_t)ksbrk(0) - (size_t)kheap_end;
				kheap_end = ksbrk(0);
				best_node = cur_node;
				kprintf("Heap enlarged. New end 0x%p\n", kheap_end);
				break;
			}
		}

		if (cur_node->status == KMALLOC_RES)
		{
			cur_node = cur_node->next;
			continue;
		}

		/* How much padding do we need for aligning the beginning? */
		size_t padding_size = 0;
		if (((size_t)cur_node + NODE_T_SIZE) % align != 0)
			padding_size = align - ((size_t)cur_node % align);

		if (cur_node->size == (size + padding_size))
		{
			/* Found a perfect node */
			best_node = cur_node;
			break;
		}
		else if (cur_node->size > (size + padding_size))
		{
			if (best_node == NULL || best_node->size > cur_node->size)
				best_node = cur_node;
		}

		cur_node = cur_node->next;
		continue;
	}

	/* best_node is now either NULL (can't allocate) or points
	 * to the best available node */
	if (best_node == NULL)
	{
		errno = ENOMEM;
		return NULL;
	}

	/* Allocate */
	struct kheap_node_t* padding = best_node;
	size_t padding_size = 0;

	if (((size_t)best_node + NODE_T_SIZE) % align != 0)
		padding_size = align - ((size_t)best_node % align);

	if (padding_size != 0 && padding_size != NODE_T_SIZE)
	{
		/* Add padding before the node, if necessary */
		best_node = (void*)((size_t)best_node+padding_size-NODE_T_SIZE);
		padding->size = padding_size - 2 * NODE_T_SIZE;
		padding->status = KMALLOC_FREE;
		padding->next = best_node;
		best_node->prev = padding;
		best_node->size -= (size_t)best_node - (size_t)padding;
	}

	/* Create padding after the node, if necessary */
	if (best_node->size != size)
	{
		padding = (void*)((size_t)best_node + NODE_T_SIZE + size);
		padding->prev = best_node;
		padding->next = best_node->next;
		padding->status = KMALLOC_FREE;
		best_node->next = padding;

		if (padding->next != NULL)
		{
			padding->size = \
				(size_t)padding->next - (size_t)padding - NODE_T_SIZE;
			padding->next->prev = padding;
		}
		else
		{
			padding->size = \
				(size_t)kheap_end - (size_t)padding - NODE_T_SIZE;
		}
	}

	best_node->size = size;
	best_node->status = KMALLOC_RES;

//	dump_kheap();
	
	return (void*)((size_t)best_node + NODE_T_SIZE);
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
	if (ptr->next == NULL && ptr->size > 2 * PAGE_SIZE)
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

static void print_kheap_node_t(struct kheap_node_t* node)
{
	kprintf("%p\t%u\t%u\t\t%s\t%x\t%x\n", node,			\
	        node->size, node->size + sizeof(struct kheap_node_t),	\
	        node->status == KMALLOC_FREE ? "free" : "used",		\
	        node->prev, node->next);
}


void dump_kheap()
{
	kprintf("**KERNEL HEAP DUMP**\n");
	struct kheap_node_t* ptr = (struct kheap_node_t*)kheap_start;
	kprintf("KHEAP: SIZE: %p START: %p\n", kheap_end - kheap_start, kheap_start);
	int i = 0;
	while (ptr != NULL && ptr < kheap_end)
	{
//		if (i % 20 == 0)
			//kprintf("NODE\t\tSIZE\tWITH NODE\tSTATUS\tPREV\tNEXT\n");
		print_kheap_node_t(ptr);
		ptr = ptr->next;
		i++;
	}
	kprintf("\n");
}
