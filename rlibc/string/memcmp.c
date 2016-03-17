/* Copyright (c) 2014 Eetu "Razbit" Pesonen
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

#include <string.h>

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
	const char* cptr1 = ptr1;
	const char* cptr2 = ptr2;

	for (; num > 0; num--)
	{
		if (*cptr1 < *cptr2)
			return -1;
		else if (*cptr1 > *cptr2)
			return 1;

		cptr1++;
		cptr2++;
	}

	return 0;
}
