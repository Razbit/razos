/* This file is a part of the RazOS project
 *
 * Razbit 2015, based on Charlie Somerville's Radium */

#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>

#define countof(x) (sizeof(x)/sizeof(*(x)))

static inline uint32_t round_down(uint32_t val, uint32_t divisor)
{
	return val - val % divisor;
}

static inline uint32_t round_up(uint32_t val, uint32_t divisor)
{
	if (val % divisor != 0)
		return val + divisor - val % divisor;
	return val;
}

#endif /* UTIL_H */
