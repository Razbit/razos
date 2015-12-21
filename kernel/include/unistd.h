/* This file is a part of the RazOS project
 *
 * unistd.h -- POSIX standard constants and types
 *
 * Razbit 2015 */

#ifndef UNISTD_H
#define UNISTD_H

#include <sys/types.h>

/* lseek -- where to start (plus offset) (whence) */
#define SEEK_SET 0 /* Start from beginning of the file */
#define SEEK_CUR 1 /* Start from current position in the file */
#define SEEK_END 2 /* Start at EOF */

/* stdio constants */
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2


off_t lseek(int fd, off_t offset, int whence);
ssize_t read(int fd, void* buf, size_t size);
ssize_t write(int fd, const void* buf, size_t size);
int close(int fd);

#endif /* UNISTD_H */
