#include <math.h>

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
