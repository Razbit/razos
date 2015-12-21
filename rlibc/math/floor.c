#include <math.h>

double floor(double x)
{
	if (isnan(x) || (fabs(x) > 9007199254740992.0)) /* 2^53 */
		return x;
	if (x < 0)
		x = ((long int)x == x ? x : x - 1);
	
	return (long int)x;
}
