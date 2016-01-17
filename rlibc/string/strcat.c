/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

char* strcat(char* dest, const char* source)
{
	char* start = dest;

	while (*start != '\0')
		start++;

	/* start now points to the null character terminating the string 'dest'*/

	while (*source != '\0')
		*start++ = *source++;

	start = '\0';

	return dest;
}

char* strncat(char* dest, const char* source, size_t num)
{
	char* start = dest;

	while (*start != '\0')
		start++;

	/* start now points to the null character terminating the string 'dest'*/

	unsigned int i = 0;

	while ((*source != '\0') && (i < num))
	{
		*start++ = *source++;
		i++;
	}

	start = '\0';

	return dest;
}

