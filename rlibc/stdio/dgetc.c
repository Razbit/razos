/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <stdio.h>
#include <unistd.h>

int dgetc(int fd)
{
	char c[1];
	if (read(fd, &(c[0]), 1) < 0)
		return EOF;

	return c[0];
}
