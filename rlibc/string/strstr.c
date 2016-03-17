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

char* strstr(const char* str1, const char* str2)
{
	unsigned int i;

	while (*str1 != '\0')
	{
		str1 = strchr(str1, *str2);
		if (!str1)
			return NULL;

		for (i = 1; i <= strlen(str2); i++)
		{
			if (str1[i] != str2[i])
				break;


		}

		if (i == strlen(str2))
			return (char*)str1;
		else
			str1++;
	}

	return NULL;
}
