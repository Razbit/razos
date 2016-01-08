/* This file is a part of the RazOS project
 *
 * setup_rlibc.h -- setup the library
 *
 * Razbit 2016 */

#include <api/razos.h>
#include <stdint.h>
#include <errno.h>

/* Set kernel parameter */
int set_kernel_param(unsigned int cmd, int param)
{
	return (int)__syscall2(SYS_SETUP, (uint32_t)cmd, (uint32_t)param);
}

/* Initialize the lib. Called from crt0.s _start */
int init_rlibc()
{
	int* errno_loc = __errno_location();
	int ret = set_kernel_param(SET_ERRNO_LOC, (int)errno_loc);

	return ret;
}
