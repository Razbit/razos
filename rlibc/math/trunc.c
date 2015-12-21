#include <math.h>

double trunc(double x)
{
    if (isnan(x))
        return x;
    if (fabs(x) > (2ull << 62))
        return x;

    return (long int)x;
}
