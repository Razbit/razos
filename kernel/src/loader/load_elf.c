/* This file is a part of the RazOS project
 *
 * load_elf.c -- Load an ELF executable
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <string.h>
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
		goto bad;

	void* ret = (void*)UCODE_BEGIN;

	Elf32_Off offset = elfhdr->e_phoff;
	int i = 0;
	for (; i < elfhdr->e_phnum; i++, offset += sizeof(Elf32_Phdr))
	{
		Elf32_Phdr* phdr = (Elf32_Phdr*)(data + offset);
		if (phdr->p_type != PT_LOAD)
			continue;

		/* Sanity check, just in case */
		if (phdr->p_memsz < phdr->p_filesz)
			goto bad;

		/* Allocate space for the segment and copy the data */
		Elf32_Word size = 0;
		for (; size < phdr->p_memsz; size += PAGE_SIZE)
		{
			void* map = NULL;

			if (phdr->p_flags & PF_W)
			{
				map = page_map(phdr->p_vaddr+size, frame_alloc(), \
				         PF_PRES | PF_USER | PF_RW, cur_task->page_dir);
			}
			else
			{
				map = page_map(phdr->p_vaddr+size, frame_alloc(), \
				         PF_PRES | PF_USER, cur_task->page_dir);
			}

			if (map == NULL)
				goto bad;
		}

		memset((void*)(phdr->p_vaddr), 0, phdr->p_memsz);
		memcpy((void*)(phdr->p_vaddr), (void*)(data + phdr->p_offset), \
		       phdr->p_filesz);

		ret = (void*)(phdr->p_vaddr + phdr->p_memsz);
	}

	return ret;

bad:
	errno = ENOEXEC;
	return (void*)-1;
}
