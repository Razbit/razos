/* This file is a part of the RazOS project
 *
 * gdt.h -- Global Descriptor Table
 *
 * Razbit 2015 (based on Charlie Somerville's Radium) */

#ifndef GDT_H
#define GDT_H

#include <sys/types.h>

/* GDT selectors */
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE 0x18
#define GDT_USER_DATA 0x20
#define GDT_TSS 0x28

/* GDT privilege (ring) */
#define GDT_KERNEL 0
#define GDT_USER 3

/* GDT entry type */
#define GDT_DATA 0
#define GDT_CODE 1

void gdt_set_entry(uint32_t sel, uint32_t base, uint32_t limit, \
                   uint32_t priv, uint32_t type);
void gdt_set_tss(uint32_t sel, uint32_t base, uint32_t limit);
void gdt_reload(); /* gdt.s */
void gdt_init();

#endif /* GDT_H */
