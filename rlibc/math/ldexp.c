/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <math.h>
#include <stdint.h>
#include <errno.h>

typedef union
{
	double d;
	struct
	{
		uint32_t mantissa_l : 32;
		uint32_t mantissa_h : 20;
		uint32_t exponent : 11;
		uint32_t sign : 1;
	} parts;
} double_cast;

double ldexp(double x, int exp)
{
	double_cast dc;
	dc.d = x;

	if (isnan(x)) return x;

	exp += dc.parts.exponent - 1023;

	if (exp > 1023)
	{
		errno = ERANGE;
		if (x < 0) return -HUGE_VAL;
		else       return HUGE_VAL;
	}
	else if (exp < -1023)
	{
		return 0.0;
	}

	dc.parts.exponent = exp + 1023;

	return dc.d;
}
