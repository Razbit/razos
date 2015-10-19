/* This file is a part of the RazOS project
 *
 * console.h -- kernel i/o system
 *
 * Razbit 2014 */

#ifndef CONSOLE_H
#define CONSOLE_H

void kclear_scr();

int kputchar(char c);
int kputs(char *c);

int kprintf(const char* fmt, ...);

#endif /* CONSOLE_H */
