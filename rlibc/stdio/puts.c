/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int puts(const char *s)
{
	write(STDOUT_FILENO, s, strlen(s));
	return write(STDOUT_FILENO, "\n", 1);
}
