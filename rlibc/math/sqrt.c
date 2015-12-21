#include <math.h>

double sqrt(double x)
{
    __asm__("fsqrt" : "+t" (x));
    return x;
}
