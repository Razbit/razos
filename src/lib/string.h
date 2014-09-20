/* A partial implementation of the C-string library
 *
 * Razbit 2014 */

#ifndef H_STRING
#define H_STRING

#include "stddef.h"
#include "stdint.h"

void* memset(void* ptr, char value, int num);
void* memcpy(void* dest, const void* source, size_t num);
void* memmove(void* dest, const void* source, size_t num);

char* strcpy(char* dest, const char* source);
char* strncpy(char* dest, const char* source, size_t num);
char* strcat(char* dest, const char* source);
char* strncat(char* dest, const char* source, size_t num);

#endif /* H_STRING */
