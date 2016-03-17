/* This file is a part of the RazOS project
 *
 * mktime.c -- mktime()
 *
 * Razbit 2016 */

#include <time.h>

static const int mdays[2][12] =
{
	{31,28,31,30,31,30,31,31,30,31,30,31},
	{31,29,31,30,31,30,31,31,30,31,30,31} /* A leap year */
};

#define isleap(y) (((y) % 4 == 0 && (y) % 100 != 0) || ((y) % 400 == 0))

/* Tomohiko Sakamoto's day_of_week (a tiny bit modified).
 * y = years, m = [0,11], d = [1,31] */
static int get_dow(int y, int m, int d)
{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	if (m < 2)
		y--;
	return (y + y/4 -y/100 + y/400 + t[m] + d) % 7;
}

time_t mktime(struct tm* tmp)
{
	/* Normalize the tm structure to match the POSIX-specified ranges.
	 * tm_wday and tm_yday are ignored */

	/* For determining leap years, fix this POSIX brainfart */
	tmp->tm_year += 1900;

	struct tm orig = *tmp;

	tmp->tm_sec = orig.tm_sec % 60;
	tmp->tm_min += orig.tm_sec / 60;
	tmp->tm_hour += tmp->tm_min / 60;
	tmp->tm_min = tmp->tm_min % 60;
	tmp->tm_mday += tmp->tm_hour / 24;
	tmp->tm_hour = tmp->tm_hour % 24;

	/* Now tm_sec, tm_min, tm_hour should be set correctly.
	 * Next we set tm_mon and tm_mday */
	
	int mon = orig.tm_mon;
	while (tmp->tm_mday > mdays[isleap(tmp->tm_year)][mon % 12])
	{
		tmp->tm_mon++;
		tmp->tm_mday -= mdays[isleap(tmp->tm_year)][mon % 12];
		mon++;
		
		if (tmp->tm_mon >= 12)
		{
			tmp->tm_year++;
			tmp->tm_mon -= 12;
		}
	}

	while (tmp->tm_mon >= 12)
	{
		tmp->tm_year++;
		tmp->tm_mon -= 12;
	}

	/* Calculate tm_wday, tm_yday */
	tmp->tm_wday = get_dow(tmp->tm_year, tmp->tm_mon, tmp->tm_mday);

	tmp->tm_yday = tmp->tm_mday;
	
	/* Add whole months */
	for (int m = 0; m < tmp->tm_mon; m++)
		tmp->tm_yday += mdays[isleap(tmp->tm_year)][m];

	/* Calculate seconds since the Epoch */
	time_t ret = tmp->tm_sec;
	ret += 60 * tmp->tm_min;
	ret += 60*60 * tmp->tm_hour;
	ret += 24*60*60 * (tmp->tm_yday-1);

	tmp->tm_year--;
	int leaps = (tmp->tm_year/4)-(tmp->tm_year/100)+(tmp->tm_year/400);
	leaps -= (1970/4)-(1970/100)+(1970/400);
	tmp->tm_year++;

	ret += 365*24*60*60 * (tmp->tm_year - 1970);
	ret += 24*60*60 * leaps;

	tmp->tm_year -= 1900;
	return ret;
}

