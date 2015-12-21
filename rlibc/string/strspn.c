/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

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

