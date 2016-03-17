/* This file is a part of the RazOS project
 *
 * Defines kernel's C-side entry point
 *
 * Razbit 2014-2016
 */

#include <sys/types.h>
#include <asm/multiboot.h>
#include <asm/system.h>
#include <console.h>
#include <colors.h>
#include <kassert.h>
#include <time.h>

#include "gdt.h"
#include "drivers/idt.h"
#include "drivers/pit.h"
#include "drivers/kb.h"
#include "mm/paging.h"
#include "mm/task.h"
#include "mm/sched.h"
#include "fs/initrd.h"
#include "fs/stdio_vfs.h"
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

	init_stdin();
	init_stdout();
	init_stderr();

	init_kb();
	
	/* Load initrd files to the ramfs
	 * open, close, creat, read, write, lseek can now access initrd */
	init_initrd((void*)(*(uint32_t*)mb->mods_addr));
	
	kputs("RazOS kernel initialized, starting init..");

	/* Uncomment and set arguments/env variables when/if needed. */
	//char* argv[] = {"arg0", NULL};
	//char* envp[] = {"VAR=VAL", NULL};
	
	uint32_t* ret = execve("rash", NULL, NULL);

	if (ret != NULL)
	{
		kputs("Switching to user mode..");
		sched_begin(ret[2], ret[1], ret[0]);
	}
	
	kputs("\n==HALTED==");

	return 0xDEADBABE;
}
