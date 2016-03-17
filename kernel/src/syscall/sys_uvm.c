/* sys_uvm.c -- user virtual memory syscalls */

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

#include <api/razos.h>
#include "syscall.h"

#include "../mm/uvm.h"

uint32_t sys_brk(struct registers_t* regs)
{
	return (uint32_t)uvm_brk((void*)REG_ARG1(regs));
}

uint32_t sys_sbrk(struct registers_t* regs)
{
	return (uint32_t)uvm_sbrk((intptr_t)REG_ARG1(regs));
}
