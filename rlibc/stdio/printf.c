/* printf.c -- printf() */

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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

int printf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int ret = vdprintf(STDOUT_FILENO, fmt, args);

	va_end(args);
	return ret;
}
