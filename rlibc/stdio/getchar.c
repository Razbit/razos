/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <stdio.h>
#include <unistd.h>

int getchar(void)
{
	return dgetc(STDIN_FILENO);
}
