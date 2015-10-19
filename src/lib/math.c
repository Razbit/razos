#include <stdint.h>
#include <math.h>

double fabs(double x)
{
    return (x < 0.0 ? -x : x);
}

double sqrt(double x)
{
    __asm__("fsqrt" : "+t" (x));
    return x;
}

double frexp(double x, int *exp)
{
    if (isinf(x)) return x;
    *exp = 0;
    while (x >= 1)
    {
        x /= 2;
        *exp += 1;
    }
    return x;
}

double ldexp(double x, int exp)
{
    double m;
    if (exp < 0) m = 1 / (double)(1 << -exp);
    else         m = 1 << exp;
    return x * m;
}

/* C99 and newer standards only */
static double trunc(double x)
{
    if (isnan(x))
        return x;
    if (fabs(x) > (2ull << 62))
        return x;

    return (long int)x;
}

double floor(double x)
{
    if (isnan(x))
        return x;
    if (fabs(x) > (2ull << 62))
        return x;
    if (x < 0)
        x = ((long int)x == x ? x : x - 1);
    
    return (long int)x;
}

double ceil(double x)
{
    if (isnan(x))
        return x;
    if (fabs(x) > (2ull << 62))
        return x;
    if (x > 0)
        x = ((long int)x == x ? x : x + 1);
    return (long int)x;
}

double modf(double x, double *integer)
{
    if (isinf(x))
        return (x < 0.0 ? -0 : 0);
    *integer = trunc(x);
    x -= *integer;
    return x;
}

double fmod(double x, double y)
{
    return x - (y * trunc(x/y));
}

int32_t pow(int32_t base, int32_t exp)
{
	if (base == 0)
		return 0;

	if (base == 1)
		return base;

	int32_t ret = base;

	if (base > 0) /* negative exponent done when needed.. */
	{
		int i;
		for (i = 0; i < exp; i++)
			ret *= base;
	}

	return ret;
}
