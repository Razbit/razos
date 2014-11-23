/* This file is a part of the RazOS project
 *
 * kassert.h -- C assert macro, in the kernel world
 *
 * Razbit 2014 */

#ifndef KASSERT_H
#define KASSERT_H

#include <panic.h>
#include <sprintf.h>

#define kassert(as)                                                      \
({                                                                       \
    if (!(as))                                                           \
    {                                                                    \
        char str[1024];                                                  \
        sprintf(str, "KASSERT: %s:%d: %s: assertion `" #as "` failed.\n",\
                    __FILE__, __LINE__, __func__);                       \
        panic(str);                                                      \
        }                                                                \
})

#endif /* KASSERT_H */

