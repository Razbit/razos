/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#ifndef STDLIB_H
#define STDLIB_H

/* Types */
typedef struct
{
    int quot;
    int rem;
} div_t;

typedef struct
{
    long quot;
    long rem;
} ldiv_t;

/* Conversion functions */
/* C-String to integer */
int atoi (const char *str);

/* Math functions */
div_t div(int numer, int denom);

ldiv_t ldiv(long numer, long denom);

int abs(int n);

long labs(long n);

#endif /* STDLIB_H */
