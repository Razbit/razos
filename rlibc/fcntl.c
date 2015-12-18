/* This file is a part of the RazOS project
 *
 * fcntl.h -- File control options
 *
 * Razbit 2015 */

#include "stdint.h"
#include "fcntl.h"

#include <api/razos.h>


int open(const char* name, int oflag)
{
	return (int)__syscall2(SYS_OPEN, (uint32_t)name, (uint32_t)oflag);
}

int creat(const char* name, int mode)
{
	return (int)__syscall2(SYS_CREAT, (uint32_t)name, (uint32_t)mode);
}
