/* elf.h -- The Execubale ELF format (32-bit i386 only) */

/* Copyright (c) 2015 Eetu "Razbit" Pesonen
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

#ifndef ELF_H
#define ELF_H

#include <sys/types.h>

/* Load an elf program image to memory.
 * We assume that the user-space is empty (no memory allocated),
 * make sure that we do the free-ing in do_execve().
 * Returns end of program image, or (void*)-1 on error */
void* load_elf(void* data);

/* Standard ELF data types */
/* 32-bit ELF */
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;

/* ELF file header. Appears at the start of every ELF file */
#define EI_NIDENT 16

typedef struct
{
	uint8_t    e_ident[EI_NIDENT]; /* Magic number etc */
	Elf32_Half e_type;		/* Object file type */
	Elf32_Half e_machine;	/* Architecture */
	Elf32_Word e_version;	/* Object file version */
	Elf32_Addr e_entry;		/* Entry point vaddress */
	Elf32_Off  e_phoff;		/* Program header table offset */
	Elf32_Off  e_shoff;		/* Section header table offset */
	Elf32_Word e_flags;		/* Processor-specific flags */
	Elf32_Half e_ehsize;	/* ELF header size (bytes) */
	Elf32_Half e_phentsize;	/* Program header entry size */
	Elf32_Half e_phnum;		/* Number of program headers */
	Elf32_Half e_shentsize;	/* Section header entry size */
	Elf32_Half e_shnum;		/* Number of section headers */
	Elf32_Half e_shstrndx;	/* Section header string table index */
} Elf32_Ehdr;

/* Fields in the e_ident array. The EI_* macros are indices into the
 * array. The macros under each EI_* macro are the values the byte
 * may have. */
#define EI_MAG0 0x0       /* File identification byte 0 index */
#define ELFMAG0 0x7f      /* Magic number byte 0 */

#define EI_MAG1	0x1       /* File identification byte 1 index */
#define ELFMAG1	'E'       /* Magic number byte 1 */

#define EI_MAG2	0x2       /* File identification byte 2 index */
#define ELFMAG2	'L'       /* Magic number byte 2 */

#define EI_MAG3	0x3       /* File identification byte 3 index */
#define ELFMAG3	'F'	      /* Magic number byte 3 */

#define EI_CLASS 0x4      /* File class byte index */
#define ELFCLASSNONE 0x0  /* Invalid class */
#define ELFCLASS32 0x1    /* 32-bit objects */

#define EI_DATA 0x5       /* Data encoding byte index */
#define ELFDATANONE	0x0   /* Invalid data encoding */
#define ELFDATA2LSB 0x1   /* 2's complement, little endian */

#define EI_VERSION 0x6    /* File version byte index. */
/* Has to match EV_CURRENT (below) */

#define EI_OSABI 0x7      /* OS ABI identification */
#define ELFOSABI_SYSV 0x0 /* UNIX System V ABI */

#define EI_ABIVERSION 0x8 /* ABI version */

#define EI_PAD 0x9        /* Start of padding */

/* Legal values for e_type (object file type) */
#define ET_NONE 0x0       /* No file type */
#define ET_EXEC 0x2       /* Executable file */

/* Legal values for e_machine (architecture) */
#define EM_NONE 0x0       /* No machine */
#define EM_386 0x3        /* Intel 80386 */

/* Legal values for e_version */
#define EV_NONE 0x0       /* Invalid ELF version */
#define EV_CURRENT 0x1    /* Current ELF version */

/* Program header */
typedef struct
{
	Elf32_Word p_type;   /* Type of segment */
	Elf32_Off  p_offset; /* Offset from beginning of the file */
	Elf32_Addr p_vaddr;  /* Virtual address to load this segment at */
	Elf32_Addr p_paddr;  /* Physical address to load at, ignored */
	Elf32_Word p_filesz; /* Size of the segment in the file */
	Elf32_Word p_memsz;  /* Size of the segment in memory */
	Elf32_Word p_flags;  /* Flags for the segment */
	Elf32_Word p_align;  /* Alignment */
} Elf32_Phdr;

/* Legal values for p_type */
#define PT_NULL 0x0 /* Entry is unused */
#define PT_LOAD 0x1 /* Loadable program segment */

/* Legal values for p_flags */
#define PF_X 0x1 /* Executable */
#define PF_W 0x2 /* Writeable */
#define PF_R 0x4 /* Readable */

#endif /* ELF_H */
