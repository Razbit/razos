/* open.c -- open() from fcntl */

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

#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>

#include <api/razos.h>

int open(const char* name, int oflag, ...)
{
	mode_t mode = 0;
	va_list args;
	va_start(args, oflag);
	if (oflag & O_CREAT)
		mode = va_arg(args, mode_t);
	va_end(args);

	return (int)__syscall3(SYS_OPEN, (uint32_t)name, (uint32_t)oflag, \
	                       (uint32_t)mode);
}
