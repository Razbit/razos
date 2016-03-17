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

size_t strcspn(const char* str1, const char* str2)
{
	size_t ret = 0;
	const char* temp = str2;

	while (*str1 != '\0')
	{
		while (*temp != '\0')
		{
			if (*str1 == *temp)
				return ret;
			temp++;
		}

		temp = str2;
		str1++;
		ret++;
	}

	return ret;
}

size_t strspn(const char* str1, const char* str2)
{
	size_t ret = 0;
	const char* temp = str2;

	while (*str1 != '\0')
	{
		while (*temp != '\0')
		{
			if (*temp == *str1)
				ret++;
			temp++;
		}

		temp = str2;
		str1++;
	}

	return ret;
}
