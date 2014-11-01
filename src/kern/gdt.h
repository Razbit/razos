/* This file is a part of the RazOS project
 *
 * gdt.h -- the x86 GDT
 *
 * Razbit 2014 */

#ifndef GDT_H
#define GDT_H

#include "../lib/stdint.h"

void init_gdt();

/* Contains a GDT entry */
struct gdt_entry_t
{
    uint16_t low_lim;   /* Low 16 bits of the segment limit */
    uint16_t low_base;  /* Low 16 bits of the base address */
    uint8_t mid_base;   /* Base address bits 16..23 */
    uint8_t access_fl;  /* 0..3: Type (code/data); 4: Descriptor type;
                         * 5..6: Privilege (ring); 7: Present (y/n) */
    uint8_t granularity; /* 0..3: Segment limit bits 16..19; 4..5: zero;
                         * 6: Operand size (16/32) 7: Byte/kbyte */
    uint8_t high_base;  /* Base address bit 24..31 */
}__attribute__((__packed__)); /* Don't change alignment */

/* Used to tell the CPU where the GDT is */
struct gdt_ptr_t
{
    uint16_t limit; /* Max size of the GDT (minus 1) */
    uint32_t base;  /* Address of the first gdt_entry struct */
}__attribute__((__packed__));

#endif /* GDT_H */
