/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

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

