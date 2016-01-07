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
#define O_ACCMODE 0x03

/* Access modes used by open/fcntl */
#define O_RDONLY 0x01
#define O_WRONLY 0x02
#define O_RDWR   (O_RDONLY | O_WRONLY)

/* Flags for open/fcntl */
#define O_CREAT    0x0010 /* Create file if it doesn't exist */
#define O_EXCL     0x0020 /* Exclusive use */
#define O_NOCTTY   0x0040 /* Do not assign a controlling terminal */
#define O_TRUNC    0x0080 /* Set length of file to 0 at write */
#define O_APPEND   0x0100 /* Append to file when writing */
#define O_NONBLOCK 0x0200 /* Non-blocking mode */


/* Function prototypes */
int open(const char* name, int oflag, ...);
int creat(const char* name, mode_t mode);

#endif /* FCNTL_H */
