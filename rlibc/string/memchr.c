/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

void* memchr(const void* ptr, int value, size_t num)
{
	const char* cptr = ptr;

	for (; num > 0; num--)
	{
		if (*cptr == value)
			return (char*)cptr;
		cptr++;
	}

	return NULL;
}

