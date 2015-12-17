/* This file is a part of the RazOS project
 *
 * exec.c -- do_execve(), the implementation behind the sys_execve()
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <kmalloc.h>
#include <unistd.h>
#include <fcntl.h>
#include <console.h>

#include "../mm/task.h"
#include "../mm/paging.h"

#include "elf.h"


int do_execve(char* path, char** argv, char** envp)
{
	/* TODO: argv */
	/* TODO: envp */
	/* TODO: clean user-space */

	int fd = open(path, O_RDONLY);
	if (fd < 0)
		goto bad;
	size_t size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);

	uint8_t* buf = kmalloc(size);
	read(fd, buf, size);

	if (load_elf(buf) < 0)
		goto bad;

	kfree(buf);
	close(fd);

	return 0;
	
bad:
	return -1;
}
