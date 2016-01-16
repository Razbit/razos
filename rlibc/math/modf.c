/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <math.h>

double modf(double x, double *integer)
{
	if (isnan(x))
	{
		*integer = x;
		return x;
	}
    if (isinf(x))
	{
		*integer = x;
		return (x < 0.0 ? -0 : 0);
	}
    *integer = trunc(x);
    x -= *integer;
    return x;
}
