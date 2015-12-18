/* This file is a part of the RazOS project
 *
 * unistd.h -- POSIX standard constants and types
 *
 * Razbit 2015 */

#ifndef UNISTD_H
#define UNISTD_H

/* TODO: Iiro, tee naille typedeffeille taikoja :D  */
#ifndef SIZE_T
#define SIZE_T
typedef unsigned int size_t;
#endif

#ifndef SSIZE_T
#define SSIZE_T
typedef int ssize_t;
#endif

#ifndef OFF_T
#define OFF_T
typedef int off_t;
#endif

#ifndef PID_T
#define PID_T
typedef unsigned int pid_t;
#endif


/* lseek -- where to start (plus offset) (whence) */
#define SEEK_SET 0 /* Start from beginning of the file */
#define SEEK_CUR 1 /* Start from current position in the file */
#define SEEK_END 2 /* Start at EOF */

off_t lseek(int fd, off_t offset, int whence);
ssize_t read(int fd, void* buf, size_t size);
ssize_t write(int fd, const void* buf, size_t size);
int close(int fd);

#endif /* UNISTD_H */
