#include <math.h>

double fmod(double x, double y)
{
    return x - (y * trunc(x/y));
}
