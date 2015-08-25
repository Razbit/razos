/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <stdlib.h>
#include <ctype.h>

/* Convert C-string to integer */
int atoi (const char *str)
{
    int i = 0;

    while (isdigit(*str))
        i = i * 10 + (*str++) - '0';

    return i;
}
