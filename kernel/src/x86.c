/* x86.c -- x86 architecture-specific thingies */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
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

#include <sys/types.h>
#include <asm/x86.h>

/* Get, set and check control registers */

void set_cr0(uint32_t flag)
{
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0) :: "memory");
	cr0 |= flag;
	__asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0) : "memory");
}

void set_cr3(uint32_t flag)
{
	uint32_t cr3;
	__asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3) :: "memory");
	cr3 |= flag;
	__asm__ __volatile__("mov %0, %%cr3" :: "r"(cr3) : "memory");
}

void set_cr4(uint32_t flag)
{
	uint32_t cr4;
	__asm__ __volatile__("mov %%cr4, %0" : "=r"(cr4) :: "memory");
	cr4 |= flag;
	__asm__ __volatile__("mov %0, %%cr4" :: "r"(cr4) : "memory");
}

void clear_cr0(uint32_t flag)
{
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0) :: "memory");
	cr0 &= ~flag;
	__asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0) : "memory");
}

void clear_cr3(uint32_t flag)
{
	uint32_t cr3;
	__asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3) :: "memory");
	cr3 &= ~flag;
	__asm__ __volatile__("mov %0, %%cr3" :: "r"(cr3) : "memory");
}

void clear_cr4(uint32_t flag)
{
	uint32_t cr4;
	__asm__ __volatile__("mov %%cr4, %0" : "=r"(cr4) :: "memory");
	cr4 &= ~flag;
	__asm__ __volatile__("mov %0, %%cr4" :: "r"(cr4) : "memory");
}

uint32_t get_cr0()
{
	uint32_t cr0;
	__asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0) :: "memory");
	return cr0;
}

uint32_t get_cr3()
{
	uint32_t cr3;
	__asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3) :: "memory");
	return cr3;
}

uint32_t get_cr4()
{
	uint32_t cr4;
	__asm__ __volatile__("mov %%cr4, %0" : "=r"(cr4) :: "memory");
	return cr4;
}
