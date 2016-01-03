/* This file is a part of the RazOS project
 *
 * sys/stat.h -- file status information. 
 *
 * Razbit 2016 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "vfs.h"

int stat(const char* path, struct stat* buf)
{
	int fd = open(path, O_RDONLY);
	return fstat(fd, buf);
}

int fstat(int fd, struct stat* buf)
{
	
}
