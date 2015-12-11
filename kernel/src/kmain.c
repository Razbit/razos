/* This file is a part of the RazOS project
 *
 * Define kernel's C-side entry point
 *
 * Razbit 2014, 2015
 */

#include <console.h>
#include "gdt.h"
#include "interrupt/idt.h"
#include "interrupt/pit.h"
#include "mm/paging.h"
#include "mm/kernel_page.h"
#include "mm/kheap.h"
#include "mm/task.h"
#include "syscall/syscall.h"

#include <fcntl.h>
#include <unistd.h>
#include "fs/vfs.h"
#include "fs/ramfs.h"
#include "fs/initrd.h"

#include <kassert.h>
#include <sys/types.h>
#include <asm/system.h>
#include <asm/multiboot.h>
#include <kmalloc.h>

int kmain(struct multiboot_info* mb, uint32_t esp)
{
	/* Init routines here.. */
	kclear_scr();

	gdt_init();
	idt_init();

	kprintf("Stack is at 0x%p\n", esp);

	pit_set_freq(100);

	paging_init(mb);

	/* kmalloc(), kfree() available from this point on */
	task_init();
	syscall_init();
	init_initrd((void*)(*(uint32_t*)mb->mods_addr));
	/* open_vfs, close_vfs, creat_vfs, read_vfs, write_vfs, lseek_vfs */

	
	kprintf("RazOS kernel initialized, starting init..\n");

	kprintf("\n==HALTED==");

	for(;;);

	return 0xDEADBABE;
}

