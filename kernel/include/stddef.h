/* This file is a part of the RazOS project.
 *
 * Razbit 2014 */

#ifndef STDDEF_H
#define STDDEF_H

/* null pointer */
#ifdef NULL
#undef NULL
#endif
#define NULL ((void *) 0)

/* signed type for pointer subtraction */
#ifndef PTRDIFF_T
#define PTRDIFF_T
typedef long int ptrdiff_t;
#endif /* _PTRDIFF_T */

/* unsigned type of size of something */
#ifndef SIZE_T
#define SIZE_T
typedef long unsigned int size_t;
#endif /* _SIZE_T */

#endif /* STDDEF_H */
