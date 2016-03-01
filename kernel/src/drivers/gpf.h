/* This file is a part of the RazOS project
 *
 * gpf.h -- a handler for general protection faults
 *
 * Razbit 2016 */

#ifndef GPF_H
#define GPF_H

#include <sys/types.h>

void gpf_handler(uint32_t err, uint32_t eip);

#endif /* GPF_H */
