/* This file is a part of the RazOS project
 *
 * do_exit.c -- the exit -syscall. The stdlib exit() is in crt0.s
 *
 * Razbit 2015 */

#include <stdint.h>
#include <api/razos.h>

void do_exit(uint32_t status)
{
	__syscall1(SYS_EXIT, status);
}
