/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
//#include <stdio.h>

#define MALLOC_FREE 1
#define MALLOC_RES 0

#define PAGE_SIZE 4096

static char *heap_start = NULL;
static char *heap_end   = NULL;

static inline uint32_t round_up(uint32_t val, uint32_t divisor)
{
	return (val-1 + divisor) & ~(val-1);
}

/* Size of heap_node_t must be 16 bytes for intended alignment. */
struct heap_node_t
{
	size_t size;
	uint32_t status; /* MALLOC_FREE or MALLOC_RES */
	struct heap_node_t* next;
	struct heap_node_t* prev;
};


static void create_heap(size_t size)
{
	heap_start = (char*)sbrk(size);
	if (heap_start == (void*)(-1))
	{
		heap_start = NULL;
	}
	else
	{
		heap_end = sbrk(0);
		struct heap_node_t* start = (struct heap_node_t*)heap_start;
		start->size = (size_t)(heap_end - (size_t)(start+1));
		start->status = MALLOC_FREE;
		start->next = NULL;
		start->prev = NULL;
	}
}
/* size_t size is the new, smaller size of curnode:
*  size <= curnode->size - sizeof(struct heap_node_t), so at least 
*  a heap_node_t struct with size 0 fits in the padding. */
static void create_padding_node(struct heap_node_t* curnode, size_t size)
{
	struct heap_node_t* padding;
	padding = (struct heap_node_t*)((size_t)(curnode+1) + size);
	padding->prev = curnode;
	padding->next = curnode->next;
	if (padding->next != NULL)
	{
		padding->size = (size_t)padding->next - (size_t)(padding+1);
		padding->next->prev = padding;
	}
	else
	{
		padding->size = (size_t)heap_end - (size_t)(padding+1);
	}
	padding->status = MALLOC_FREE;
	curnode->next = padding;
	curnode->size = size;
}

void* malloc(size_t size)
{
	if (size == 0)
		return NULL;
	
	size = round_up(size, sizeof(struct heap_node_t));

	/* Make sure we have a heap */
	if (heap_start == NULL)
		create_heap(PAGE_SIZE + size);
	
	struct heap_node_t* curnode = (struct heap_node_t*)heap_start;

	void* ret = NULL;

	/* Find a decent node */
	while (curnode != NULL)
	{
		if (curnode->next == NULL) /* Last node */
		{
			/* Enlarge the heap. */
			if (curnode->size < size)
			{
				if (sbrk(size - curnode->size) == (void*)(-1))
					break;
				heap_end = sbrk(0);
				curnode->size = (size_t)heap_end - (size_t)(curnode+1);
			}
		}
		
		if (curnode->status == MALLOC_RES || curnode->size < size)
		{
			curnode = curnode->next;
			continue;
		}

		/* If we reach this, we have found a decent node. */
		
		/* Pad if we need to. */
		if (curnode->size > size)
			create_padding_node(curnode, size);
		
		curnode->status = MALLOC_RES;

		ret = (void*)((char*)curnode + sizeof(struct heap_node_t));
		break;
	}

	return ret;
}

/* Unify continuous free space following the freed memory node */
static void unify_fwd(struct heap_node_t* ptr)
{
	ptr->size += ptr->next->size + sizeof(struct heap_node_t);

	ptr->next = ptr->next->next;

	if (ptr->next != NULL)
		ptr->next->prev = ptr;
}

void free(void *ptr)
{
	if (ptr == NULL)
		return;

	/* The given ptr points to the beginning of usable space rather than
	 * to the beginning of the heap_node_t struct */
	
	ptr -= sizeof(struct heap_node_t);
	struct heap_node_t* nodeptr = ptr;

	/* Mark as free */
	nodeptr->status = MALLOC_FREE;

	/* Unify free memory nodes */
	if (nodeptr->next != NULL && nodeptr->next->status == MALLOC_FREE)
		unify_fwd(nodeptr);

	if (nodeptr->prev != NULL && nodeptr->prev->status == MALLOC_FREE)
		unify_fwd(nodeptr->prev);

	/* Last element -> free some of the heap */
	if (nodeptr->next == NULL)
	{
		nodeptr->size = PAGE_SIZE;
		heap_end = ((char*)(nodeptr+1)) + PAGE_SIZE;
		brk((void*)heap_end);
	}
}

void *realloc(void *ptr, size_t size)
{
	if (ptr == NULL)
		return malloc(size);

	if (size == 0)
		return NULL;
	
	struct heap_node_t* nodeptr = ((struct heap_node_t*)ptr) - 1;
	void *ret = ptr;

	if (size < nodeptr->size) /* Shrinking the area of allocated memory. */
	{
		size_t new_node_size = round_up(size, sizeof(struct heap_node_t));
		if (new_node_size < nodeptr->size)
			create_padding_node(nodeptr, new_node_size);
	}
	if (size > nodeptr->size) /* Enlarging the area of allocated memory. */
	{
		ret = malloc(size);
		memcpy(ret, ptr, nodeptr->size);
		free(ptr);
	}

	return ret;
}

void *calloc(size_t nelem, size_t elsize)
{
	if (nelem == 0 || elsize == 0)
		return NULL;

	void *ret = malloc(nelem*elsize);
	memset(ret, 0, nelem*elsize);
	return ret;
}
/*
static void print_heap_node_t(struct heap_node_t* node)
{
	printf("%p\t%u\t%lu\t\t%s\t%p\t%p\n", node,			\
	        node->size, node->size + sizeof(struct heap_node_t),	\
	        node->status == MALLOC_FREE ? "free" : "used",		\
	        node->prev, node->next);
}

void dump_heap()
{
	struct heap_node_t* ptr = (struct heap_node_t*)heap_start;
	printf("**HEAP DUMP**\nKHEAP: SIZE: %p START: %p\n", (char*)(heap_end-heap_start), heap_start);
	int i = 0;
	while (ptr != NULL)
	{
		if (i % 20 == 0)
			printf("NODE\t\tSIZE\tWITH NODE\tSTATUS\tPREV\tNEXT, %d\n", 1);
		print_heap_node_t(ptr);
		ptr = ptr->next;
		i++;
	}
}
*/
