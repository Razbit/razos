#include <math.h>

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
