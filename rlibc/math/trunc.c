#include <math.h>

double trunc(double x)
{
	if (isnan(x) || (fabs(x) > 9007199254740992.0)) /* 2^53 */
		return x;

	return (long int)x;
}
