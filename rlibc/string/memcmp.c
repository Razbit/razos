/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
	const char* cptr1 = ptr1;
	const char* cptr2 = ptr2;

	for (; num > 0; num--)
	{
		if (*cptr1 < *cptr2)
			return -1;
		else if (*cptr1 > *cptr2)
			return 1;

		cptr1++;
		cptr2++;
	}

	return 0;
}

