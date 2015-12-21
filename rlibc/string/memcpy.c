/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>
#include <stdint.h>

void* memcpy(void* dest, const void* source, size_t num)
{
	uint8_t* dst = dest;
	const uint8_t* src = source;

	if (num == 0)
		return dest;

	while (num)
	{
		*dst++ = *src++;
		num--;
	}

	return dest;
}

