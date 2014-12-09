/* Define kernel's C-side entry point
 * 
 * Razbit 2014 
 */

#include "kio.h"
#include "gdt.h"
#include "interrupt/idt.h"
#include "interrupt/irq.h"
#include "interrupt/timer.h"
#include "interrupt/kb.h"
#include "mm/paging.h"

#include <stdint.h>
#include <kassert.h>

#include "mm/kmalloc.h"

int kmain(struct multiboot *mboot_ptr)
{
	/* Init routines here.. */
	kclear_scr();

    init_gdt();
    init_idt();
    init_irq();

    __asm__ __volatile__("sti"); /* Enable IRQs */

    init_kb();
    init_pit(1);
    init_paging();

    uint8_t* a = kmalloc(8);
    uint16_t* b = kmalloc(8);
    uint32_t* c = kmalloc(8);
    uint32_t* d = kmalloc(8);

    *a = 32;
    *b = 16;
    *c = 8;
    *d = 0xFFFFFF8;
    
    kprintf("address of %c: %p value: %p\n", 'a', a, *a);
    kprintf("address of %c: %p value: %p\n", 'b', b, *b);
    kprintf("address of %c: %p value: %p\n", 'c', c, *c);
    kprintf("address of %c: %p value: %p\n", 'd', d, *d);
    
    for(;;);
	kprintf("\n==HALTED==");

    __asm__ __volatile__("cli"); /* Disable IRQs */
    
	return 0xDEADBABE;
}

