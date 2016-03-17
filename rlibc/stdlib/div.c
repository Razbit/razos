/* This file is a part of the RazOS project
 *
 * Itara20 2015 */

#include <stdlib.h>

div_t div(int numer, int denom)
{
    div_t result;
    result.quot = numer / denom;
    result.rem  = numer - (denom * result.quot);
    return result;
}

ldiv_t ldiv(long numer, long denom)
{
    ldiv_t result;
    result.quot = numer / denom;
    result.rem  = numer - (denom * result.quot);
    return result;
}
