/* This file is a part of the RazOS project
 *
 * panic.h -- a crappy kernel panic
 *
 * Razbit 2014 */

#include <panic.h>
#include <asm/system.h>
#include <stdarg.h>
#include <vsprintf.h>

#include <console.h>

void panic(const char* msg, ...)
{
    cli();
    
    char str[1024];
    va_list va;
    va_start(va, msg);

    vsprintf(str, msg, va);
    kprintf("\nKernel panic: %s\n", str);

    va_end(va);
    
    for(;;);
}
