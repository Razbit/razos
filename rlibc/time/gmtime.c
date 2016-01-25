/* This file is a part of the RazOS project
 *
 * gmtime.c -- gmtime()
 *
 * Razbit 2016 */

#include <time.h>

struct tm* gmtime(const time_t* timer)
{
	static struct tm ret =
	{
		.tm_sec = 0,
		.tm_min = 0,
		.tm_hour = 0,
		.tm_mday = 1,
		.tm_mon = 0,
		.tm_year = 70,
		.tm_isdst = -1,
	};

	ret.tm_sec = *timer;

	/* Normalize the tm structure using mktime */
	mktime(&ret);

	return &ret;
}
