/* This file is a part of the RazOS project
 *
 * kio.h -- kernel i/o system
 *
 * Razbit 2014 */

#ifndef KIO_H
#define KIO_H

void kclear_scr();

int kputchar(char c);
int kputs(char *c);

int kprintf(const char* fmt, ...);

#endif /* KIO_H */
