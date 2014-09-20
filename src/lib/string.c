/* A partial implementation of the C-string library
 *
 * Razbit 2014 */

#include "string.h"
#include "stdint.h"
#include "stddef.h"

void* memset(void* ptr, char value, int num)
{
	num--;
	char* str = ptr;
	
	for (;num >= 0; num--)
		str[num] = value;

	return ptr;
}

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
