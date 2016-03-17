/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

char* strpbrk(const char* str1, const char* str2)
{
	const char* temp = str2;

	while (*str1 != '\0')
	{
		while (*temp != '\0')
		{
			if (*str1 == *temp)
				return (char*)str1;
			temp++;
		}

		temp = str2;
		str1++;
	}

	return NULL;
}
