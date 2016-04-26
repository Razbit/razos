/* sys/sysinfo.c -- system information */

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

#include <sys/sysinfo.h>
#include <stdint.h>
#include <api/razos.h>

/* Get information from the kernel */
static inline uint32_t get_kernel_info(uint32_t cmd)
{
	return __syscall1(SYS_SETUP, (uint32_t)cmd);
}

unsigned int get_used_mem()
{
	return get_kernel_info(GET_USED_MEM);
}
