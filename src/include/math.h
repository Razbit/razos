/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#ifndef MATH_H
#define MATH_H

#include <sys/types.h>

#define isnan(x) (x != x)
#define isinf(x) (x == x && ((x - x) != 0.0)) 

double fabs(double x);

double sqrt(double x);

/* Resultant value is x = mantissa (returned value) * 2 ^ exp.
 * Mantissa is between 0.5 (inclusive) and 1.0 (exclusive). */
double frexp(double x, int *exp);

/* Returns x * 2 ^ exp. */
double ldexp(double x, int exp);

double floor(double x);

double ceil(double x);

/* Returns the fractional part of x, integer pointing to the integer part. */
double modf(double x, double *integer);

double fmod(double x, double y);

int32_t pow(int32_t base, int32_t exp);

#endif /* MATH_H */
