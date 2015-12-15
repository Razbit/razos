/* This file is a part of the RazOS project
 *
 * load_elf.c -- Load an ELF executable
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <string.h>
#include <console.h>

#include "../mm/paging.h"
#include "../mm/task.h"

#include "elf.h"


/* Load an elf program image to memory.
 * We assume that the user-space program area is empty,
 * make sure that we do the free-ing in do_exec(). */
int load_elf(void* data)
{
	
	
	/* Parse elf */
	Elf32_Ehdr* elfhdr = data;

	/* Check the file */
	if (elfhdr->e_ident[EI_MAG0] != ELFMAG0)
		goto bad;
	if (elfhdr->e_ident[EI_MAG1] != ELFMAG1)
		goto bad;
	if (elfhdr->e_ident[EI_MAG2] != ELFMAG2)
		goto bad;
	if (elfhdr->e_ident[EI_MAG3] != ELFMAG3)
		goto bad;
	
	int i;
	Elf32_Off offset;
	Elf32_Phdr* phdr;
	for (i = 0, offset = elfhdr->e_phoff; \
	     i < elfhdr->e_phnum; \
	     i++, offset += sizeof(Elf32_Phdr)
		)
	{
		phdr = (Elf32_Phdr*)(data + offset);
		if (phdr->p_type != PT_LOAD)
			continue;
		if (phdr->p_memsz < phdr->p_filesz)
			goto bad;

		/* Allocate space for the segment and copy the data */
		Elf32_Word size = 0;
		for (; size < phdr->p_memsz; size += PAGE_SIZE)
		{
			kprintf("load_elf: allocating space for code at 0x%p\n", phdr->p_vaddr+size);
			uint32_t page = page_alloc();
			if (phdr->p_flags & PF_W)
			{
				page_map(phdr->p_vaddr+size, page, \
				         PE_PRESENT | PE_USER | PE_RW);
			}
			else
			{
				page_map(phdr->p_vaddr+size, page, PE_PRESENT | PE_USER);
			}
		}
		
		memcpy((void*)(phdr->p_vaddr), (void*)(data + phdr->p_offset), \
		       phdr->p_filesz);
	}

	return 1;
	
bad:
	return -1;
}
