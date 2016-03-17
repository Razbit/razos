/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <math.h>

#define LOG2E 1.4426950408889634
#define MAX_X 709.782712893384 // LOG2E * MAX_X = 1024
double exp(double x)
{
	if (isnan(x)) return x;
	if (x > MAX_X) return INFINITY;

	double y = LOG2E * x;
	int intpart = (int)trunc(y);
	double fractional = y - intpart;

	double exp2;
	__asm__("fldl   %1\n\t"
			"f2xm1"
			: "=t"(exp2)
			: "m"(fractional)
			: "st(1)");

	return ldexp(1, intpart) * (exp2+1);
}
