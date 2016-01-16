/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#ifndef MATH_H
#define MATH_H

#define NAN       (0.0/0.0)
#define INFINITY  (1.0/0.0)
#define HUGE_VAL  INFINITY
#define HUGE_VALF HUGE_VAL
#define HUGE_VALL HUGE_VAL

#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4

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

double exp(double x);

double log(double x);

double pow(double base, double e);

#endif /* MATH_H */
