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
#include "interrupt/kb.h"
#include "mm/paging.h"
#include "mm/task.h"
#include "mm/sched.h"
#include "syscall/syscall.h"

#include <fcntl.h>
#include <unistd.h>
#include "fs/initrd.h"
#include "fs/stdio_vfs.h"

#include "loader/exec.h"

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

	init_stdin();
	init_stdout();
	init_stderr();

	init_kb();
	
	/* Load initrd files to the ramfs */
	init_initrd((void*)(*(uint32_t*)mb->mods_addr));
	
	/* open, close, creat, read, write, lseek can now access initrd */
	
	kputs("RazOS kernel initialized, starting init..\n");
	
	int ret = exec("test");
	kprintf("Execve returned %i\n", ret);
	if (ret == 0)
	{
		kputs("Starting scheduler..\n");
		sched_begin();
	}
	
	kputs("\n==HALTED==");

	return 0xDEADBABE;
}
