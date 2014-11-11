/* Define kernel's C-side entry point
 * 
 * Razbit 2014 
 */

#include "kio.h"
#include "gdt.h"
#include "interrupt/idt.h"
#include "interrupt/irq.h"
#include "interrupt/timer.h"

int kmain(struct multiboot *mboot_ptr)
{
	/* Init routines here.. */
	kclear_scr();

    init_gdt();
    init_idt();
    init_irq();

    __asm__ __volatile__("sti"); /* Enable IRQs */
    
    init_pit(10);

    kprintf("Will halt in 10 seconds.\n");
    uint32_t ticks = 0;
    while (ticks < 100)
    {
        ticks = pit_get_ticks();
    }
        
	kprintf("\n==HALTED==");

    __asm__ __volatile__("cli"); /* Disable IRQs */
    
	return 0xDEADBABE;
}

