/* This file is a part of the RazOS project.
 *
 * Razbit 2014 */

#ifndef CTYPE_H
#define CTYPE_H

int isalnum(int c);
int isalpha(int c);
int isdigit(int c);
int iscntrl(int c);
int isgrapf(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int x);

int tolower(int c);
int toupper(int c);

#endif /* CTYPE_H */
