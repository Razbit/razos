/* fcntl.c -- File control options */

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
#include <fcntl.h>
#include <stdarg.h>
#include <api/razos.h>

int fcntl(int fd, int cmd, ...)
{
	uint32_t arg;
	va_list args;
	va_start(args, cmd);

	switch(cmd)
	{
	case F_DUPFD:
	case F_SETFD:
	case F_SETFL:
		arg = va_arg(args, uint32_t);
		break;
	default:
		arg = 0;
		break;
	}

	va_end(args);

	return (int)__syscall3(SYS_FCNTL, (uint32_t)fd, (uint32_t)cmd, \
	                  (uint32_t)arg);
}
