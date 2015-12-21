#include <math.h>

double modf(double x, double *integer)
{
    if (isinf(x))
        return (x < 0.0 ? -0 : 0);
    *integer = trunc(x);
    x -= *integer;
    return x;
}
