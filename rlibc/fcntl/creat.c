/* This file is a part of the RazOS project
 *
 * creat.c -- creat() from fcntl
 *
 * Razbit 2015 */

#include <stdint.h>
#include <fcntl.h>

#include <api/razos.h>

int creat(const char* name, mode_t mode)
{
	return (int)__syscall2(SYS_CREAT, (uint32_t)name, (uint32_t)mode);
}
