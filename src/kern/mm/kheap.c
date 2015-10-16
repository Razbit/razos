/* This file is a part of the RazOS project
 *
 * kheap.c -- kernel heap system
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <console.h>
#include <util.h>
#include <string.h>

#include "paging.h"
#include "kernel_page.h"

#include "kheap.h"

static uint32_t kheap_start = 0;
static uint32_t kheap_size = 0;

#define KMALLOC_FREE 1
#define KMALLOC_RES 0

struct kheap_node_t
{
	size_t size;
	uint32_t status; /* free or res */
	struct kheap_node_t* next;
	struct kheap_node_t* prev;
};

static struct kheap_node_t* kheap_last_node = NULL;

/* Internals of the kmalloc() -family.
 * We align the start address to [align] bytes */
void* do_kmalloc(size_t size, size_t align)
{
	/* Start address is aligned to [align] that is a multiple of 8.
	 * End of the block is aligned so that the entire chunk's end is
	 * aligned to 8 bytes. */

	if (size == 0)
		return NULL;

	size = round_up(size, sizeof(struct kheap_node_t));

	/* Make sure we have a heap */
	if (kheap_start == 0)
	{
		ksbrk(size + PAGE_SIZE);
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
	while (1)
	{
		if (curnode == NULL)
			break;

		if (curnode->next == NULL)
		{
			/* TODO: what if this last node is reserved? */

			/* Enlarge the heap */
			if (curnode->size < size + PAGE_SIZE)
			{
				void* prev_end = ksbrk(0);
				void* new_end = ksbrk(size + PAGE_SIZE - curnode->size);
				curnode->size += (size_t)(new_end - prev_end);
			}
		}

		if (curnode->status == KMALLOC_RES)
		{
			curnode = curnode->next;
			continue;
		}

		size_t padding_amount = 0;
		if (((size_t)curnode % align) != 0)
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
		if (padding_amount != 0 && padding_amount != sizeof(struct kheap_node_t))
		{
			/* Set the padding node */
			curnode = (void*)curnode + padding_amount - sizeof(struct kheap_node_t);
			padding->size = padding_amount - 2*sizeof(struct kheap_node_t);
			padding->status = KMALLOC_FREE;
			padding->next = curnode;
			curnode->prev = padding;
		}

		/* Pad if we need to */
		if (curnode->size != size)
		{
			padding = (void*)curnode + sizeof(struct kheap_node_t) + size;
			padding->prev = curnode;
			padding->next = curnode->next;
			if (padding->next)
			{
				padding->size = (size_t)padding->next - (size_t)padding \
					- sizeof(struct kheap_node_t);
			}
			else
			{
				padding->size = (kheap_start + kheap_size) \
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

/* Unify continuous free space following the freed memory node */
static void unify_fwd(struct kheap_node_t* ptr)
{
	ptr->size += ptr->next->size + sizeof(struct kheap_node_t);

	ptr->next = ptr->next->next;

	if (ptr->next)
		ptr->next->prev = ptr;
}

/* Unify continuous free space before the freed memory node */
static void unify_bwd(struct kheap_node_t* ptr)
{
	if (ptr->prev)
	{
		ptr->prev->next = ptr->next;
		ptr->prev->size += ptr->size + sizeof(struct kheap_node_t);
	}

	ptr->next->prev = ptr->prev;
}

/* Internals of the kfree() */
void do_kfree(void* _ptr)
{
	/* The given ptr points to the beginning of usable space rather than
	 * to the beginning of the hkeap_node_t struct */

	_ptr -= sizeof(struct kheap_node_t);
	struct kheap_node_t* ptr = _ptr;


	/* Mark as free */
	ptr->status = KMALLOC_FREE;

	/* Unify free memory nodes */
	if (ptr->next && ptr->next->status == KMALLOC_FREE)
		unify_fwd(ptr);

	if (ptr->prev && ptr->prev->status == KMALLOC_FREE)
		unify_bwd(ptr);

	/* Last element -> free some of the heap */
	if (ptr->next == NULL)
	{
		kheap_last_node = ptr;

		if (ptr->size > 2*PAGE_SIZE)
		{
			kbrk((void*)(kheap_start + kheap_size - ptr->size));
		}
	}
}

/* Kernel's brk()-ish functionality */
int kbrk(void* addr)
{
	/* Take care of the paging behind kernel's heap, much like
	 * the brk() syscall, i.e. maintain a contiguous heap of memory
	 * available for kmalloc()'s use. */

	if (addr >= (void*)KHEAP_MAX)
		return -1;

	/* First allocation? */
	if (kheap_start == 0)
	{
		kheap_start = (uint32_t)kernel_page_alloc_zeroed();
		kheap_size += PAGE_SIZE;
	}

	while (kheap_start + kheap_size < (uint32_t)addr)
	{
		/* We increase the size of the kernel heap */
		kernel_page_alloc_zeroed();
		kheap_size += PAGE_SIZE;
	}

	while (kheap_start + kheap_size - PAGE_SIZE > (uint32_t)addr)
	{
		/* Free memory from the end of the kernel heap */
		kernel_page_free((void*)round_down(kheap_start+kheap_size-1, PAGE_SIZE));
		kheap_size -= PAGE_SIZE;
		kheap_last_node->size -= PAGE_SIZE;
	}

	/* If we have freed the whole kheap, we have to create a new one.
	 * We do that in the next call to kbrk() or ksbrk() */
	if (kheap_size == 0)
		kheap_start = 0;

	return 0;
}

/* Kernel's sbrk()-ish functionality */
void* ksbrk(size_t increment)
{
	/* First allocation? */
	if (kheap_start == 0)
	{
		kheap_start = (uint32_t)kernel_page_alloc_zeroed();
		kheap_size += PAGE_SIZE;

		if (increment >= PAGE_SIZE)
			increment -= PAGE_SIZE;
		else
			increment = 0;
	}

	/* Truncate if we try to allocate over the heap boundary */
	while (kheap_start + kheap_size + increment > KHEAP_MAX)
		increment -= PAGE_SIZE;

	/* Allocate memory to the heap, page by page */
	size_t i = 0;
	for (i = 0; i < increment; i += PAGE_SIZE)
	{
		kernel_page_alloc_zeroed();
		kheap_size += PAGE_SIZE;
	}

	return (void*)(kheap_start + kheap_size);
}

static void print_kheap_node_t(struct kheap_node_t* node)
{
	kprintf("KHEAP_NODE: %p %u %u, %s; %p %p\n", node,			   \
	        node->size, node->size+sizeof(struct kheap_node_t),	   \
	        node->status == KMALLOC_FREE ? "free" : "used",		   \
	        node->prev, node->next);
}

void dump_kheap()
{
	kprintf("**KERNEL HEAP DUMP**\n");
	struct kheap_node_t* ptr = (void*)kheap_start;
	kprintf("KHEAP: SIZE: %p START: %p\n", kheap_size, kheap_start);
	while (ptr)
	{
		print_kheap_node_t(ptr);
		ptr = ptr->next;
	}
	kprintf("\n");
}
