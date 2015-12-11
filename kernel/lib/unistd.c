/* This file is a part of the RazOS project
 *
 * unistd.h -- POSIX standard constants and types
 *
 * Razbit 2015 */

#include <unistd.h>

#include <sys/types.h>
#include "../src/fs/vfs.h"

off_t lseek(int fd, off_t offset, int whence)
{
	return lseek_vfs(fd, offset, whence);
}

ssize_t read(int fd, void* buf, size_t size)
{
	return read_vfs(fd, buf, size);
}

ssize_t write(int fd, const void* buf, size_t size)
{
	return write_vfs(fd, buf, size);
}

