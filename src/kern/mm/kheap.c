/* This file is a part of the RazOS project
 *
 * kheap.c -- kernel heap system
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <console.h>
#include <util.h>

#include "paging.h"
#include "kernel_page.h"

#include "kheap.h"

static uint32_t kheap_start = 0;
static uint32_t kheap_size = 0;

/* Internals of the kmalloc() -family */
void* do_kmalloc(size_t size, int align)
{
    //
}

/* Internals of the kfree() */
void do_kfree(void* ptr)
{
    //
}

/* Kernel's brk()-ish functionality */
int kbrk(void* addr)
{
    /* Take care of the paging behind kernel's heap, much like
     * the brk() syscall, i.e. maintain a contiguous heap of memory
     * available for kmalloc()'s use. */

    if (addr >= KHEAP_END)
        return -1;
    
    /* First allocation? */
    if (kheap_start == 0)
    {
        kheap_start = kernel_page_alloc_zeroed();
        kheap_size += PAGE_SIZE;
    }

    while (kheap_start + kheap_size < (uint32_t)addr)
    {
        /* We increase the size of the kernel heap */
        kernel_page_alloc_zeroed_at();
        kheap_size += PAGE_SIZE;
    }
    
    while (kheap_start + kheap_size > (uint32_t)addr)
    {
        /* Free memory from the end of the kernel heap */
        kernel_page_free(round_down(kheap_start+kheap_size-1, PAGE_SIZE));
        kheap_size -= PAGE_SIZE;
    }

    return 0;
}

/* Kernel's sbrk()-ish functionality */
void* ksbrk(size_t increment)
{
    /* First allocation? */
    if (kheap_start == 0)
    {
        kheap_start = kernel_page_alloc_zeroed();
        kheap_size += PAGE_SIZE;

        if (increment >= PAGE_SIZE)
            increment -= PAGE_SIZE;
        else
            increment = 0;
    }

    /* page-align */
    increment = round_up(increment, PAGE_SIZE);
    
    /* Truncate if we try to allocate over the heap boundary */
    while (kheap_start + kheap_size + increment > KHEAP_END)
        increment -= PAGE_SIZE;
    
    size_t i = 0;
    for (i = 0; i < increment; i += PAGE_SIZE)
    {
        kernel_page_alloc_zeroed();
        kheap_size += PAGE_SIZE;
    }
    
    return kheap_start + kheap_size;
}
