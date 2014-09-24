/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#ifndef VSPRINTF_H
#define VSPRINTF_H

#include "stdarg.h"
#include "string.h"

int vsprintf (char* s, const char* format, va_list arg);

#endif /* VSPRINTF_H */
