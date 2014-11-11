/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <stdarg.h>
#include <string.h>
#include <vsprintf.h>
#include <sprintf.h>

int sprintf(char* str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    int ret = vsprintf(str, fmt, args);

    va_end(args);

    return ret;
}
