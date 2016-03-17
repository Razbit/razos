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

char* strtok(char* str, const char* delimiters)
{
	static char* last;
	if (str)
		last = NULL;

	int i = 0;
	int len = strlen(delimiters);

	if(len == 0)
		return NULL;

	/* if the original string has nothing left */
	if(!str && !last)
		return NULL;

	if(str && !last)
		last = str;

	/* start of the substr */
	char* p_start = last;

	while (1)
	{
		for (i = 0; i < len; i++)
		{
			if (*p_start == delimiters[i])
			{
				p_start++;
				break;
			}
		}

		if (i == len)
		{
			last = p_start;
			break;
		}
	}

	if (*last == '\0')
	{
		last = NULL;
		return NULL;
	}


	/* find the end of the substring, replace the delimiter with null */
	while(*last != '\0')
	{
		for(i = 0; i < len; i++)
		{
			if(*last == delimiters[i])
			{
				*last = '\0';
				break;
			}
		}

		last++;
		if (i < len)
			break;
	}

	return p_start;
}
