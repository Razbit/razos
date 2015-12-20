/* This file is a part of the RazOS project
 *
 * Razbit 2015 */

#include <stdlib.h>

int atoi (const char *str)
{
	int i = 0;

	while (*str >= 0x30 && *str <= 0x39) /* isdigit(*str) */
		i = i * 10 + (*str++) - '0';

	return i;
}
