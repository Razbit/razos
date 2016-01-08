/* This file is a part of the RazOS project
 *
 * errno.h -- errno
 *
 * Razbit 2016 */

#ifndef ERRNO_H
#define ERRNO_H

#include <errno_defs.h>

int* __get_errno_loc();

/* Get errno from cur_task or use the internal kernel errno */
#define errno (*__get_errno_loc())

#endif /* ERRNO_H */
