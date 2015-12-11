/* This file is a part of the RazOS project
 *
 * fcntl.h -- File control options
 *
 * Razbit 2015 */

#include <fcntl.h>
#include <sys/types.h>

int open(const char* name, int oflag)
{
	return open_vfs(name, oflag);
}

int creat(const char* name, int mode)
{
	return creat_vfs(name, mode);
}

int close(int fd)
{
	return close_vfs(fd);
}
