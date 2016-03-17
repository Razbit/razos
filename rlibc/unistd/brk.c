/* brk.c -- brk() and sbrk() syscalls */

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

#include <unistd.h>
#include <stdint.h>

#include <api/razos.h>

int brk(void* addr)
{
	return (int)__syscall1(SYS_BRK, (uint32_t)addr);
}

void* sbrk(intptr_t incr)
{
	return (void*)__syscall1(SYS_SBRK, (uint32_t)incr);
}
