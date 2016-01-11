/* This file is a part of the RazOS project.
 *
 * Razbit 2014 */

#ifndef CTYPE_H
#define CTYPE_H

int isalnum(char c);
int isalpha(char c);
int isdigit(char c);
int iscntrl(char c);
int isgrapf(char c);
int islower(char c);
int isprint(char c);
int ispunct(char c);
int isspace(char c);
int isupper(char c);
int isxdigit(char x);

int tolower(char c);
int toupper(char c);

#endif /* CTYPE_H */
