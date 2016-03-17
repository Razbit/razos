/* Copyright (c) 2014, 2016 Eetu "Razbit" Pesonen
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

int strcmp(const char* str1, const char* str2)
{
	int diff = (int)strlen(str1) - (int)strlen(str2);
	if (diff != 0)
		return diff;

	while (1)
	{
		if (*str1 < *str2)
			return -1;
		if (*str1 > *str2)
			return 1;
		if (*str1 == '\0') /* Actually, if both are null */
			return 0;

		str1++;
		str2++;
	}
}

int strncmp(const char* str1, const char* str2, size_t num)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	for (; num > 0; num--)
	{
		/* str1 is too short */
		if ((len1 < num) && (len1 < len2))
			return -1;
		/* str2 is too short */
		if ((len2 < num) && (len2 < len1))
			return 1;
		if (*str1 < *str2)
			return -1;
		if (*str1 > *str2)
			return 1;
		if (*str1 == '\0') /* Actually, if both are null */
			return 0;

		str1++;
		str2++;
		len1--;
		len2--;
	}

	return 0;
}
