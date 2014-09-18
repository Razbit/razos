/* A partial implementation of the C-string library
 *
 * Razbit 2014 */

#include "string.h"

void* memset(void * ptr, char value, int num)
{
	num--;
	char* str = ptr;
	
	for (;num >= 0; num--)
		str[num] = value;

	return ptr;
}
