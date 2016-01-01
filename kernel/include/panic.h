/* This file is a part of the RazOS project
 *
 * panic.h -- kernel panic
 *
 * Razbit 2014 */

#ifndef PANIC_H
#define PANIC_H

#include <stdarg.h>

void panic(const char* msg, ...) __attribute__ ((noreturn));

#endif /* PANIC_H */
