/* ctime.h -- ctime, asctime */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

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
