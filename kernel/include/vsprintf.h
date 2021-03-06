/* Copyright (c) 2014 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

/* The basis of printf() is here, the vsprintf().
 * Following is what this implementation can (will be able to) do.
 * This is by no means a C99 compliant printf(), but it will
 * fullfill its purpose.
 *
 * Format specifier prototype:
 * %[flags][width][.prec][len]specifier
 *
 * flags: -     Left justify                                        [ok]
 *        0     Pad left with zeros                                 [ok]
 *        +     Always print a sign                                 [ok]
 *        (spc) Write a space if no sign is written                 [ok]
 *        #     Used w/ specifiers o, x or X the value is           [ok]
 *              preceded by 0, 0x or 0X respectively.
 *
 * width: (num) Min. number of chars to be printed, pad w/ blanks.  [ok]
 *        *     Width is specified as an int in the argument list.  [ok]
 *
 * .prec: (num) For integers (d, i, o, u, x, X): specifies the min. [ok]
 *              number of digits to be written, padded w/ leading
 *              zeros if needed. Not truncated. Precision of 0
 *              means that no character for 0 is printed.
 *              For s: max number of chars to be printed.           [ok]
 *              If the period is specified without an explicit
 *              value, 0 is assumed.
 *        *     Precision is specified as an int in the arg list.   [ok]
 *
 *              d i       u o x X   c    s     p     n
 * len:   (n/a) int       int       char char* void* int*           [ok]
 *        h     short     short                      short*         [ok]
 *        l     long      long                       long*          [ok]
 *
 * spec:  d, i       signed decimal                                 [ok]
 *        u          unsigned decimal                               [ok]
 *        o          octal                                          [ok]
 *        x, X       lower and higher case hexadecimal              [ok]
 *        c          unsigned char                                  [ok]
 *        s          C-string                                       [ok]
 *        p          a pointer                                      [ok]
 *        n          the number of chars printed so far is stored   [ok]
 *                   to this location. Nothing printed.
 *        %          a % character (0x25) is printed.               [ok]
 */

#ifndef VSPRINTF_H
#define VSPRINTF_H

#include <stdarg.h>
#include <string.h>

int vsprintf (char* str, const char* format, va_list arg);

#endif /* VSPRINTF_H */
