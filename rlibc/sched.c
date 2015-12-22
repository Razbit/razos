#include <api/razos.h>
#include <sched.h>

void sched_yield()
{
	__syscall0(SYS_SCHED_YIELD);
}
