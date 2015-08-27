/* This file is a part of the RazOS project
 *
 * POSIX and standard C type definitions
 *
 * Razbit 2014 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* The following are from POSIX */

#ifndef SSIZE_T
#define SSIZE_T
typedef int32_t ssize_t;
#endif

#ifndef OFF_T
#define OFF_T
typedef int32_t off_t;
#endif

typedef uint32_t gid_t;
typedef uint32_t uid_t;
typedef uint32_t ino_t;
typedef uint32_t mode_t;

typedef uint32_t pid_t;

#endif /* TYPES_H */
