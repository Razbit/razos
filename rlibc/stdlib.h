/* This file is a part of the RazOS project
 *
 * stdlib.h -- C99-ish stdlib
 *
 * Razbit 2015 */

#ifndef STDLIB_H
#define STDLIB_H

#define _NEED_NULL
#define _NEED_SIZE_T
#define _NEED_WCHAR_T
#include <sys/types.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define RAND_MAX 32767

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

/* Functions */
div_t div(int numer, int denom);

ldiv_t ldiv(long numer, long denom);

int abs(int n);

long labs(long n);


int rand(void);

void srand(unsigned int seed);

void *bsearch(const void *key, const void *base, size_t nitems, size_t size,
	       int (*compar)(const void *, const void *));

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));

int atoi (const char *str);

double strtod(const char *str, char **endptr);

double atof(const char *str);

void exit(int status); /* int arch/../crt0.s */

#endif /* STDLIB_H */
