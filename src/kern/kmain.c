/* This file is a part of the RazOS project
 *
 * Define kernel's C-side entry point
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
#include "fs/fs.h"
#include "fs/initrd.h"
#include "mm/kmalloc.h"
#include "mm/detect.h"

#include <kassert.h>
#include <sys/types.h>
#include <asm/system.h>
#include <asm/multiboot.h>
#include <dirent.h>

extern uint32_t placement_addr;
uint32_t ini_esp = 0;

int kmain(struct multiboot_info* mb, uint32_t esp)
{
    ini_esp = esp;
    
	/* Init routines here.. */
	kclear_scr();

    init_gdt();
    init_idt();
    init_irq();

    dump_memdetect(mb);
    kprintf("Stack is at 0x%p\n", ini_esp);
    
    /* Find initrd. We aren't doing anything with it yet, tho */
    kassert(mb->mods_count > 0);
    uint32_t initrd_loc = *((uint32_t*)mb->mods_addr);
    uint32_t initrd_end = *(uint32_t*)(mb->mods_addr+4);
    placement_addr = initrd_end;
    
    sti();
    
    init_kb();
    init_pit(1);
    
    init_paging(mb);
       
	kprintf("\n==HALTED==");

    cli();
    
	return 0xDEADBABE;
}

