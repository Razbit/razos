/* This file is a part of the RazOS project
 *
 * descriptor_tables.h -- initializes the x86 descriptor tables (GDT, IDT)
 * and defines the default ISR and IRQ handlers
 *
 * Razbit 2014 */

#include "descriptor_tables.h"
#include "gdt.h"
#include "idt.h"

/* Initialization: zero all ISRs, init GDT and IDT */
void init_descr_tables()
{
    init_gdt();
    init_idt();
}




