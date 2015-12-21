/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>


char* strchr(const char* str, char value)
{
	while (*str != '\0')
	{
		if (*str == value)
			return str;
		str++;
	}

	return NULL;
}

char* strrchr(const char* str, char value)
{
	const char* end = str;

	while (*end != '\0')
		end++;

	while (end >= str)
	{
		if (*end == value)
			return end;
		end--;
	}

	return NULL;

}

