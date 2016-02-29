/* This file is a part of the RazOS project
 *
 * gpf.c -- a handler for general protection faults
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <console.h>
#include <string.h>
#include <panic.h>

/* Masks for selector error code */
#define GPF_EXT 0x00000001 /* External or internal? */
#define GPF_GDT (0x0 << 1) /* table selector 00b -> GDT */
#define GPF_IDT (0x1 << 1) /* table selector 01b OR 11b -> IDT */
#define GPF_LDT (0x2 << 1) /* table selector 10b -> LDT */
#define GPF_IDX 0x0000FFF8 /* Index in the descriptor */

void gpf_handler(uint32_t err)
{
//	for(;;);
	if (err != 0)
	{
		/* GPF is segment related */
		if ((err & GPF_IDT) == GPF_IDT)
		{
			kprintf("Unhandled interrupt (%u)\n", (err & GPF_IDX) >> 3);
			return;
		}

		char errstr[35];
		strcpy(errstr, "GPF: index %u in ");
		
		if (err & GPF_GDT)
			strcat(errstr, "GDT");
		else
			strcat(errstr, "LDT");
		
		if (err & GPF_EXT)
			strcat(errstr, " (external)");

		panic(errstr, (err & GPF_IDX) >> 3);
	}
	else
	{
		panic("General protection fault");
	}
}
