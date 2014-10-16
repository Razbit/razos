/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#ifndef VSPRINTF_H
#define VSPRINTF_H

#include "stdarg.h"
#include "string.h"

/* The basis of printf() is here, the vsprintf().
 * Following is what this implementation can (will be able to) do.
 * This is by no means a C99 compliant printf(), but it will
 * fullfill its purpose.
 *
 * Format specifier prototype:
 * %[flags][width][.prec][len]specifier
 *
 * flags: -     Left justify
 *        0     Pad left with zeros
 *        +     Always print a sign
 *        (spc) Write a space if no sign is written
 *        #     Used w/ specifiers o, x or X the value is preceded by
 *              0, 0x or 0X respectively.
 *
 * width: (num) Min. number of chars to be printed, padded with blanks.
 *        *     Width is specified as an int in the argument list.
 *
 * .prec: (num) For integers (d, i, o, u, x, X): specifies the min.
 *              number of digits to be written, padded w/ leading
 *              zeros if needed. Not truncated. Precision of 0
 *              means that no character for 0 is printed.
 *              For g and G: max number of significant digits to
 *              be printed.
 *              For s: max number of chars to be printed.
 *              If the period is specified without an explicit
 *              value, 0 is assumed.
 *        *     Precision is specified as an int in the arg list.
 *
 *              d i       u o x X   c    s     p     n
 * len:   (n/a) int16_t   uint16_t  char char* void* int16_t*
 *        h     int16_t   uint16_t                   int16_t*
 *        l     int32_t   uint32_t                   int32_t*
 *
 * spec:  d, i       signed decimal
 *        u          unsigned decimal
 *        o          octal
 *        x, X       lower and higher case hexadecimal
 *        c          unsigned char
 *        s          C-string
 *        p          a pointer
 *        n          the number of chars printed so far is stored
 *                   to this location. Nothing printed.
 *        %          a % character (0x25) is printed.
 */

int vsprintf (char* str, const char* format, va_list arg);

#endif /* VSPRINTF_H */
