#ifndef ERRNO_H
#define ERRNO_H

extern int *__errno_location(void);
#define errno (*__errno_location())

#define EDOM 1
#define ERANGE 2
#define EILSEQ 3

#endif /* ERRNO_H */
