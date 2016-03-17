/* This file is a part of the RazOS project
 *
 * fcntl.h -- File control options
 *
 * Razbit 2015, 2016 */

#ifndef FCNTL_H
#define FCNTL_H

#define _NEED_MODE_T
#define _NEED_OFF_T
#define _NEED_PID_T
#include <sys/types.h>

#define _NEED_MODE_T_VALUES
#include <sys/stat.h>
#undef _NEED_MODE_T_VALUES

#define _NEED_WHENCE_VALUES
#include <unistd.h>
#undef _NEED_WHENCE_VALUES

/* Mask for access modes */
#define O_ACCMODE  0x03

/* Access modes used by open/fcntl */
#define O_RDONLY   0x01
#define O_WRONLY   0x02
#define O_RDWR     (O_RDONLY | O_WRONLY)

/* File status flags for open/fcntl */
/* Creation flags */
#define O_CF_MASK  0x00F0 /* Mask for the following flags */
#define O_CREAT    0x0010 /* Create file if it doesn't exist */
#define O_EXCL     0x0020 /* Exclusive use */
#define O_NOCTTY   0x0040 /* Do not assign a controlling terminal */
#define O_TRUNC    0x0080 /* Set length of file to 0 at write */

/* Status flags */
#define O_SF_MASK  0x0F00 /* Mask for the following flags */
#define O_APPEND   0x0100 /* Append to file when writing */
#define O_NONBLOCK 0x0200 /* Non-blocking mode */

/* Mask for all the above O_* flags */
#define O_MASK     (O_ACCMODE | O_CF_MASK | O_SF_MASK)

/* File descriptor flags */
#define FD_MASK    0xF000 /* Mask for the below flags */
#define FD_CLOEXEC 0x1000 /* Close FD on call to exec() */
#define O_CLOEXEC  FD_CLOEXEC

/* fcntl commands */
#define F_DUPFD    0x0001 /* Duplicate FD to lowest-numbered avail */
#define F_GETFD    0x0002 /* Get file descriptor flags */
#define F_SETFD    0x0004 /* Set file descriptor flags */
#define F_GETFL    0x0008 /* Get file status flags */
#define F_SETFL    0x0010 /* Set file status flags */

/* Function prototypes */
int open(const char* name, int oflag, ...);
int creat(const char* name, mode_t mode);
int fcntl(int fd, int cmd, ...);

#endif /* FCNTL_H */
