#ifndef ERRNO_H
#define ERRNO_H

/* errno numbers in kernel headers */
#include <errno_defs.h>

extern int *__errno_location(void);
#define errno (*__errno_location())

#endif /* ERRNO_H */
