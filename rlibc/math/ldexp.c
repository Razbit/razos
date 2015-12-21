#include <math.h>

double ldexp(double x, int exp)
{
    double m;
    if (exp < 0) m = 1 / (double)(1 << -exp);
    else         m = 1 << exp;
    return x * m;
}
