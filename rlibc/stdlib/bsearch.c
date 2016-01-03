/* This file is a part of the RazOS project
 *
 * Itara20 2015 */

#include <stdlib.h>

void *bsearch (const void *key, const void *base, size_t nitems, 
               size_t size, int (*compar)(const void *, const void *))
{
	if (nitems == 0)
		return NULL;
	
	size_t half = nitems/2;
	const char *middle = base + half*size;
	int pos = compar(middle, key);

	if (pos > 0)
		return bsearch(key, base, half, size, compar);
	else if (pos < 0)
		return bsearch(key, middle+size, half, size, compar);
	
        return (void*)middle;
}

