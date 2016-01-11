/* This file is a part of the RazOS project
 *
 * Razbit 2014, 2016 */

#ifndef STDARG_H
#define STDARG_H

/* I'll let GCC do these for me.. */

#define _NEED_VA_LIST
#include <sys/types.h>

#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

#endif /* STDARG_H */
