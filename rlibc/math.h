/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#ifndef MATH_H
#define MATH_H

#define isnan(x) (x != x)
#define isinf(x) (x == x && ((x - x) != 0.0)) 

double fabs(double x);

double sqrt(double x);

double frexp(double x, int *exp);

double ldexp(double x, int exp);

double trunc(double x);

double floor(double x);

double ceil(double x);

double modf(double x, double *integer);

double fmod(double x, double y);

#endif /* MATH_H */
