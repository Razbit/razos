/* This file is a part of the RazOS project
 *
 * load_elf.c -- Load an ELF executable
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <string.h>
#include <console.h>
#include <errno.h>

#include "../mm/paging.h"
#include "../mm/task.h"

#include "elf.h"


/* Load an elf program image to memory.
 * We assume that the user-space program area is empty,
 * make sure that we do the free-ing in do_exec(). */
void* load_elf(void* data)
{
	/* Parse elf */
	Elf32_Ehdr* elfhdr = data;

	/* Check the file */
	if ((elfhdr->e_ident[EI_MAG0] != ELFMAG0)
	    || (elfhdr->e_ident[EI_MAG1] != ELFMAG1)
	    || (elfhdr->e_ident[EI_MAG2] != ELFMAG2)
	    || (elfhdr->e_ident[EI_MAG3] != ELFMAG3))
	{
		errno = ENOEXEC;
		return (void*)-1;
	}

	void* ret = (void*)UCODE_BEGIN;
	
	Elf32_Off offset;
	Elf32_Phdr* phdr;
	for (int i = 0, offset = elfhdr->e_phoff; \
	     i < elfhdr->e_phnum; \
	     i++, offset += sizeof(Elf32_Phdr))
	{
		phdr = (Elf32_Phdr*)(data + offset);
		if (phdr->p_type != PT_LOAD)
			continue;
		if (phdr->p_memsz < phdr->p_filesz)
		{
			errno = ENOEXEC;
			return (void*)-1;
		}

		/* Allocate space for the segment and copy the data */
		Elf32_Word size = 0;
		for (; size < phdr->p_memsz; size += PAGE_SIZE)
		{
			/* TODO: check if page_map() fails */
			if (phdr->p_flags & PF_W)
			{
				page_map(phdr->p_vaddr+size, frame_alloc(), \
				         PF_PRES | PF_USER | PF_RW, cur_task->page_dir);
			}
			else
			{
				page_map(phdr->p_vaddr+size, frame_alloc(), \
				         PF_PRES | PF_USER, cur_task->page_dir);
			}
		}

		memcpy((void*)(phdr->p_vaddr), (void*)(data + phdr->p_offset), \
		       phdr->p_filesz);

		ret = (void*)(phdr->p_vaddr + phdr->p_memsz);
	}

	return ret;
}
