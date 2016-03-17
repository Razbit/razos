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

#include <stdint.h>
#include <fcntl.h>

#include <api/razos.h>

int creat(const char* name, mode_t mode)
{
	return (int)__syscall2(SYS_CREAT, (uint32_t)name, (uint32_t)mode);
}
