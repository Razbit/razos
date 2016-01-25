/* This file is a part of the RazOS project
 *
 * time.h -- time-related stuff
 *
 * Razbit 2016 */

/* TODO: timezones, dst */

#ifndef TIME_H
#define TIME_H

#define _NEED_CLOCK_T
#define _NEED_SIZE_T
#define _NEED_TIME_T
#define _NEED_NULL

#include <sys/types.h>

/* Must be 1,000,000 on XSI -conformant systems, but this ain't one */
#define CLOCKS_PER_SEC 1000

struct tm
{
	int tm_sec;   /* seconds [0,60] (allows for leap second) */
	int tm_min;   /* minutes [0,59] */
	int tm_hour;  /* hours [0,23] */
	int tm_mday;  /* day of month [1,31] */
	int tm_mon;   /* month of year [0,11] */
	int tm_year;  /* years since (before) 1900 */
	int tm_wday;  /* day of week [0,6] (sunday = 0) */
	int tm_yday;  /* day of year [0,365] */
	int tm_isdst; /* 1 if DST in effect, 0 if not. -1 if unknown */
};

/* return number of cpu-ticks used by the program since beginning of
 * program execution */
clock_t clock(void);

/* Get seconds since epoch. Return to *timer too, if not NULL */
time_t time(time_t* timer);

/* Convert broken-down time to seconds since epoch, fill tm_ptr */
time_t mktime(struct tm* tm_ptr);

/* Get difference in seconds between beg and end */
double difftime(time_t end, time_t beg);


/* Convert seconds since epoch to broken-down UTC time */
struct tm* gmtime(const time_t* time);

/* Convert seconds since epoch to broken-down local timezone time */
struct tm* localtime(const time_t* time);

/* Convert broken-down time to Www Mmm dd hh:mm:ss yyyy\n\0 */
char* asctime(const struct tm* tm_ptr);

/* Convert seconds since epoch to Www Mmm dd hh:mm:ss yyyy\n\0 */
char* ctime(const time_t* time);

/* Convert broken-down time to string str in format described in fmt */
size_t strftime(char* str, size_t max, const char* fmt,	\
                const struct tm* tm_ptr);

#endif /* TIME_H */
