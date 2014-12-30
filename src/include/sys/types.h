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
typedef long int ssize_t;
#endif

#ifndef OFF_T
#define OFF_T
typedef long int off_t;
#endif

typedef long unsigned int gid_t;
typedef long unsigned int uid_t;
typedef long unsigned int ino_t;
typedef long int off_t;

#endif /* TYPES_H */
