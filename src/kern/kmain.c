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
	task_init();
	syscall_init();

	/* kmalloc(), kfree() available from this point on */

	char* mem = kmalloc(12);

	kprintf("\n==HALTED==");

	for(;;);

	return 0xDEADBABE;
}

