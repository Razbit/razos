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

char* strchr(const char* str, int value)
{
	while (*str != '\0')
	{
		if (*str == value)
			return (char*)str;
		++str;
	}
	/* The terminating null byte is considered a part of the string. */
	if (value == '\0')
		return (char*)str;

	return NULL;
}

char* strrchr(const char* str, int value)
{
	const char* end = str;

	while (*end != '\0')
		end++;

	while (end >= str)
	{
		if (*end == value)
			return (char*)end;
		end--;
	}

	return NULL;

}
