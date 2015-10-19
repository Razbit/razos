/* This file is a part of the RazOS project
 *
 * kassert.h -- C assert macro, in the kernel world
 *
 * Razbit 2014 */

#ifndef KASSERT_H
#define KASSERT_H

#include <panic.h>
#include <sprintf.h>

/* If the assertion fails, we panic */
#define kassert(as)	\
({ \
	if (!(as)) \
	{ \
		panic("KASSERT: %s:%d: %s: assertion `" #as "` failed.\n", \
		      __FILE__, __LINE__, __func__); \
	} \
})

#endif /* KASSERT_H */
