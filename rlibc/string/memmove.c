/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>
#include <stdint.h>

void* memmove(void* dest, const void* source, size_t num)
{
	if (num == 0)
		return dest;

	/* when source is higher than dest, we can just use memcpy() */
	if (source > dest)
		return memcpy(dest, source, num);

	else
	{
		uint8_t* dst = dest;
		const uint8_t* src = source;

		num--;

		dst += num;
		src += num;

		while (num)
		{
			*dst-- = *src--;
			num--;
		}

		return dest;
	}
}

