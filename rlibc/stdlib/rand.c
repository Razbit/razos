/* This file is a part of the RazOS project
 *
 * Itara20 2015 */

#include <stdlib.h>

static unsigned long next = 1; /* For the rand implementation. */

int rand(void)
{
	next = next * 1103515245 + 12345;
	return((unsigned)(next/65536) % 32768);
}

void srand(unsigned int seed)
{
	next = seed;
}

