/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

size_t strlen(const char* str)
{
	size_t ret = 0;
	while (*str++ != '\0')
		ret++;

	return ret;
}
