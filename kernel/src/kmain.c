/* Defines kernel's C-side entry point */

/* Copyright (c) 2014-2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#include <sys/types.h>
#include <asm/multiboot.h>
#include <asm/system.h>
#include <console.h>
#include <colors.h>
#include <kassert.h>
#include <time.h>
#include <fcntl.h>
#include <kmalloc.h>
#include <memdump.h>

#include "gdt.h"
#include "drivers/idt.h"
#include "drivers/pit.h"
#include "drivers/kb.h"
#include "drivers/ata_lba.h"
#include "mm/paging.h"
#include "mm/task.h"
#include "mm/sched.h"
#include "fs/vfs.h"
#include "fs/initrd.h"
#include "fs/stdio_vfs.h"
#include "fs/devfs.h"
#include "fs/fat/fat.h"
#include "loader/exec.h"

int kmain(struct multiboot_info* mb, uint32_t esp)
{
	/* Init routines here.. */
	clear_scr();

	sched_halt();

	gdt_init();
	idt_init();

	sti();

	kprintf("Multiboot info found at %p\n", mb);
	kprintf("Stack is at %p\n", esp);

	pit_set_freq(CLOCKS_PER_SEC);

	paging_init(mb);

	/* kmalloc(), kfree() available from this point on */

	task_init();
	syscall_init();
	devfs_init();
	ramfs_init();
	init_stdio();
	init_kb();


	/* Load initrd files to the ramfs
	 * open, close, creat, read, write, lseek can now access initrd */
	init_initrd((void*)(*(uint32_t*)mb->mods_addr));

	kputs("RazOS kernel initialized, starting init..");

	struct fat_bpb_t* bpb = kmalloc(512);
	ata_lba_read(0, 1, (void*)bpb);
	dump_bpb(bpb);

	memdump(bpb, 64);

	/* Uncomment and set arguments/env variables when/if needed. */
	//char* argv[] = {"arg0", NULL};
	//char* envp[] = {"VAR=VAL", NULL};

	/*uint32_t* ret = execve("/rd/rash", NULL, NULL);

	if (ret != NULL)
	{
		kputs("Switching to user mode..");
		sched_begin(ret[2], ret[1], ret[0]);
		}*/

	kputs("\n==HALTED==");

	return 0xDEADBABE;
}
