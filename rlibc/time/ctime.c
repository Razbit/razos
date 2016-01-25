/* This file is a part of the RazOS project
 *
 * ctime.h -- ctime, asctime
 *
 * Razbit 2016 */

#include <time.h>
#include <stdio.h>

char* asctime(const struct tm* tmp)
{
	static const char* wday[] =
	{
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static const char* mon[] =
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	static char ret[26];

	sprintf(ret, "%.3s %.3s %2.2hi %2.2hi:%2.2hi:%2.2hi %4.4hi\n", \
	        wday[tmp->tm_wday], mon[tmp->tm_mon], tmp->tm_mday, \
	        tmp->tm_hour, tmp->tm_min, tmp->tm_sec, \
	        1900 + tmp->tm_year);

	return ret;
}
