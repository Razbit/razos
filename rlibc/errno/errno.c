#include <errno.h>

static int errno_var = 0;

int *__errno_location(void)
{
	return &errno_var;
}
