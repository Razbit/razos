#include <stdint.h>
#include <math.h>

int32_t pow(int32_t base, int32_t exp)
{
	if (base == 0)
		return 0;

	if (base == 1)
		return base;

	int32_t ret = base;

	if (base > 0) /* negative exponent done when needed.. */
	{
		int i;
		for (i = 0; i < exp; i++)
			ret *= base;
	}

	return ret;
}
