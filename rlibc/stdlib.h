/* This file is a part of the RazOS project
 *
 * stdlib.h -- C99-ish stdlib
 *
 * Razbit 2015, 2016, Itara20 2016 */

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

void *malloc(size_t size);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

void *calloc(size_t nelems, size_t elsize);

//void dump_heap();

int rand(void);

void srand(unsigned int seed);

void *bsearch(const void *key, const void *base, size_t nitems, size_t size,
	       int (*compar)(const void *, const void *));

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));

int atoi (const char *str);

double strtod(const char *str, char **endptr);

double atof(const char *str);

void exit(int status); /* in arch/../crt0.s */

/* Get value of environment variable */
char* getenv(const char* name);

/* Add or change environment variable */
int setenv(const char* envname, const char* envval, int overwrite);

/* Remove an environment variable */
int unsetenv(const char* name);

#endif /* STDLIB_H */
