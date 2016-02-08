/* This file is a part of the RazOS project
 *
 * uvm.h -- user virtual memory (mainly heap area)
 *
 * Razbit 2016 */

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
