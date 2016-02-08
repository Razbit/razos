/* This file is a part of the RazOS project
 *
 * kvm.h -- kernel virtual memory
 *
 * Razbit 2016 */

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
