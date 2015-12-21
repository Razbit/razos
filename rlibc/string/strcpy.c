/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

char* strcpy(char* dest, const char* source)
{
	while (*source != '\0')
	{
		*dest++ = *source++;
	}

	*dest = *source; /* terminating null character */

	return dest;
}

char* strncpy(char* dest, const char* source, size_t num)
{
	int i = 0;

	for (;i < num; i++)
	{
		*dest++ = *source++;
		if (*source == '\0')
			break;
	}

	return dest;
}

