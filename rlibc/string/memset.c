/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

void* memset(void* ptr, int value, int num)
{
	num--;
	char* str = ptr;

	for (;num >= 0; num--)
		str[num] = value;

	return ptr;
}
