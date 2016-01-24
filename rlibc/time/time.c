/* This file is a part of the RazOS project
 *
 * time.c -- time()
 *
 * Razbit 2016 */

#include <api/razos.h>
#include <time.h>

time_t time(time_t* timer)
{
	time_t ret = (time_t)__syscall0(SYS_TIME);
	if (timer != NULL)
		*timer = ret;
	return ret;
}
