/* except.c -- hw exceptions handling */

/* Copyright (c) 2015, 2017 Eetu "Razbit" Pesonen
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
#include <portio.h>
#include "idt.h"

static const char* msg[] = {
	"divide by zero",
	"debug",
	"NMI",
	"breakpoint",
	"overflow",
	"bound range exceeded",
	"invalid opcode",
	"device unavailable",
	"double fault",
	"coprocessor segment overrun",
	"invalid tss",
	"stack segment fault",
	"general protection fault",
	"page fault",
	"res",
	"x87 float",
	"alignment check",
	"machine check",
	"SIMD float",
	"virtualization",
	"security",

};

void except_err(uint32_t num, uint32_t err, uint32_t eip)
{
	if (num == 30)
		num = 21;

	panic("int %d %s (0x%x) at 0x%x\n", num, msg[num], err, eip);
}

void except(uint32_t num, uint32_t eip)
{
	panic("int %d %s at 0x%x\n", num, msg[num], eip);
}
