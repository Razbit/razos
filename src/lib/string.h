/* A partial implementation of the C-string library
 *
 * Razbit 2014 */

#ifndef H_STRING
#define H_STRING

#include "stddef.h"
#include "stdint.h"

void* memset(void* ptr, char value, int num);
void* memcpy(void* dest, const void* src, size_t num);


#endif /* H_STRING */
