#include <api/razos.h>
#include <wait.h>
#include <stdint.h>

uint32_t wait(int* stat_loc)
{
	return __syscall1(SYS_WAIT, (uint32_t)stat_loc);
}
