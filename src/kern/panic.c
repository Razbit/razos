/* This file is a part of the RazOS project
 *
 * panic.h -- a crappy kernel panic
 *
 * Razbit 2014 */

#include <panic.h>

#include "kio.h"

void panic(char* str)
{
    __asm__ __volatile__ ("cli");
    kprintf("KERNEL PANIC: %s\n", str);
    for(;;);
}
