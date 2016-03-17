/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <stdio.h>
#include <stdlib.h>

ssize_t dgetdelim(char **lineptr, size_t *n, int delim, int fd)
{
	*n = 64;
	ssize_t pos = 0;

	*lineptr = malloc(*n);
	if (*lineptr == NULL)
		return -1;

	for (char c = dgetc(fd); c != EOF && c != delim; c = dgetc(fd))
	{
		(*lineptr)[pos++] = c;

		/* If the buffer is about to be exceeded, reallocate. */
		if (pos >= *n)
		{
			*n *= 2;
			*lineptr = realloc(*lineptr, *n);
			if (*lineptr == NULL)
				return -1;
		}
	}

	(*lineptr)[pos] = '\0';
	return pos;
}
