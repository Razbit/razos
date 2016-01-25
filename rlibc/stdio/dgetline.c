/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <stdio.h>

ssize_t dgetline(char **lineptr, size_t *n, int fd)
{
	return dgetdelim(lineptr, n, '\n', fd);
}
