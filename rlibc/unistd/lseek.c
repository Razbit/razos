/* lseek.c -- lseek from unistd (POSIX) */

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

#include <unistd.h>
#include <stdint.h>

#include <api/razos.h>

off_t lseek(int fd, off_t offset, int whence)
{
	return (off_t)__syscall3(SYS_LSEEK, (uint32_t)fd, \
	                         (uint32_t)offset, (uint32_t)whence);
}
