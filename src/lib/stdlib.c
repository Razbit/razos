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

div_t div(int numer, int denom)
{
    div_t result;
    result.quot = numer / denom;
    result.rem  = numer % denom;
    return result;
}

ldiv_t ldiv(long numer, long denom)
{
    ldiv_t result;
    result.quot = numer / denom;
    result.rem  = numer % denom;
    return result;
}

int abs(int n)
{
    return (n < 0 ? -n : n);
}

long labs(long n)
{
    return (n < 0 ? -n : n);
}
