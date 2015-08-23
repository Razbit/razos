/* This file is a part of the RazOS project
 *
 * detect.h -- memory detection
 *
 * Razbit 2015 */

#ifndef DETECT_H
#define DETECT_H

#include <sys/types.h>
#include <asm/multiboot.h>

size_t get_avail_mem(struct multiboot_info* mb);

void dump_memdetect(struct multiboot_info* mb);

#endif /* DETECT_H */
