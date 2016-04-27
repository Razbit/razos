#include <stdio.h>
#include <time.h>

int main(void)
{
	time_t t = time(NULL);
	struct tm tm_now = *gmtime(&t);

	printf("%d-%d-%d %d:%d:%d\n",
			tm_now.tm_year + 1900,
			tm_now.tm_mon + 1,
			tm_now.tm_mday,
			tm_now.tm_hour,
			tm_now.tm_min,
			tm_now.tm_sec);
	return 0;
}
