/* This file is a part of the RazOS project
 *
 * Define kernel's C-side entry point
 *
 * Razbit 2014, 2015, 2016
 */

#include <sys/types.h>
#include <asm/multiboot.h>
#include <console.h>
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
	kclear_scr();

	gdt_init();
	idt_init();

	kprintf("Multiboot info found at 0x%p\n", mb);
	kprintf("Stack is at 0x%p\n", esp);

	pit_set_freq(CLOCKS_PER_SEC);

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
	
	char* argv[] = {"Hello", "world", NULL};
	char* envp[] = {"SHELL=rash", NULL};
	uint32_t* ret = execve("test", argv, envp);
	if (ret != NULL)
	{
		kputs("Switching to user mode..\n");
		sched_begin(ret[2], ret[1], ret[0]);
	}
	
	kputs("\n==HALTED==");

	return 0xDEADBABE;
}
