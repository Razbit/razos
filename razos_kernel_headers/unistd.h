/* This file is a part of the RazOS project
 *
 * unistd.h -- POSIX standard constants and types
 *
 * Razbit 2015 */

#ifndef _NEED_WHENCE_VALUES

#ifndef UNISTD_H /* Include guard */
#define UNISTD_H

#define _NEED_SIZE_T
#define _NEED_SSIZE_T
#define _NEED_UID_T
#define _NEED_GID_T
#define _NEED_OFF_T
#define _NEED_PID_T
#define _NEED_INTPTR_T
#define _NEED_NULL
#include <sys/types.h>

/* stdio constants */
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* Function prototypes */
off_t lseek(int fd, off_t offset, int whence);
ssize_t read(int fd, void* buf, size_t size);
ssize_t write(int fd, const void* buf, size_t size);
int close(int fd);

/* declarations only visible in rlibc */
#ifndef _RAZOS_KERNEL

pid_t fork();
int pipe(int fd[2]);

#endif /* _RAZOS_KERNEL */

#endif /* UNISTD_H */
#endif /* _NEED_WHENCE_VALUES */

/* Values for whence */
#define SEEK_SET 0 /* Start from beginning of the file */
#define SEEK_CUR 1 /* Start from current position in the file */
#define SEEK_END 2 /* Start at EOF */
