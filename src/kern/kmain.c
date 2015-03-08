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

#include <kassert.h>
#include <sys/types.h>
#include <asm/system.h>
#include <asm/multiboot.h>
#include <dirent.h>

extern uint32_t placement_addr;

int kmain(struct multiboot *mboot_ptr)
{
	/* Init routines here.. */
	kclear_scr();

    init_gdt();
    init_idt();
    init_irq();

    /* Find initrd */
    kassert(mboot_ptr->mods_count > 0);
    uint32_t initrd_loc = *((uint32_t*)mboot_ptr->mods_addr);
    uint32_t initrd_end = *(uint32_t*)(mboot_ptr->mods_addr+4);
    placement_addr = initrd_end;
    
    sti();

    init_kb();
    init_pit(1);
    
    init_paging();

    fs_root = init_initrd((void*)initrd_loc);

	struct dirent* node = NULL;
	while ((node = readdir_fs(fs_root)) != NULL)
	{
		kprintf("Found file %s", node->d_name);
		struct fs_node_t* fsnode = finddir_fs(fs_root, node->d_name);

		if ((fsnode->flags & 0x7) == FS_DIR)
			kprintf(" [directory]\n");
		else
		{
			kprintf(", contents:\n");
			char buf[256];
			ssize_t size = read_fs(fsnode, buf, 256, 0);
			kprintf("%.*s\n", size, buf);
		}
	}
    
    for(;;);
	kprintf("\n==HALTED==");

    cli();
    
	return 0xDEADBABE;
}

