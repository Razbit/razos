/* This file is a part of the RazOS project
 *
 * sys_regdump.h -- sys_regdump() syscall
 *
 * Razbit 2015 */

#include <sys/types.h>

#include <console.h>

#include "../mm/task.h"

#include "syscall.h"
#include <api/razos.h>

uint32_t sys_regdump(struct registers_t* regs)
{
	kprintf("REGDUMP: process %d\n", cur_task->pid);
	kprintf("eax 0x%8.8x ebx 0x%8.8x ecx 0x%8.8x edx 0x%8.8x\n", \
	        regs->eax, regs->ebx, regs->ecx, regs->edx);
	kprintf("esp 0x%8.8x ebp 0x%8.8x esi 0x%8.8x edi 0x%8.8x\n", \
	        regs->esp, regs->ebp, regs->esi, regs->edi);
	return 0;
}
