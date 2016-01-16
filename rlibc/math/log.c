/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <math.h>

double log(double x)
{
	double result;

	__asm__("fldln2   \n"
			"fldl   %1\n"
			"fyl2x    \n"
			: "=t"(result)
			: "m"(x));

	return result;
}
