/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#ifndef H_STRING
#define H_STRING

#include <sys/types.h>

/* Set [num] bytes of memory starting from [ptr] to [value] */
void* memset(void* ptr, char value, int num);
/* Copy block of memory of size [num] bytes from [source] to [dest] */
void* memcpy(void* dest, const void* source, size_t num);
/* Move block of memory of size [num] bytes from [source] to [dest] */
void* memmove(void* dest, const void* source, size_t num);

/* Copy string from [source] to [dest] */
char* strcpy(char* dest, const char* source);
/* Copy string from [source] to [dest], max [num] chars*/
char* strncpy(char* dest, const char* source, size_t num);
/* Concatenate copy of [source] string to [destination] */
char* strcat(char* dest, const char* source);
/* Append max [num] chars from [source] string to [dest] */
char* strncat(char* dest, const char* source, size_t num);

/* Compare the first [num] bytes of [ptr1] to [ptr2] */
int memcmp(const void* ptr1, const void* ptr2, size_t num);
/* Compare the string [str1] to [str2] */
int strcmp(const char* str1, const char* str2);
/* Compare the first [num] chars of [str1] to [str2] */
int strncmp(const char* str1, const char* str2, size_t num);

/* Return pointer to first occurence of [value] in the block of memory
 * of size [num] pointed by [ptr] */
void* memchr(const void* ptr, char value, size_t num);
/* Return pointer to first occurence of [value] in the string [str] */
char* strchr(const char* str, char value);
/* Return pointer to last occurence of [value] in the string [str] */
char* strrchr(const char* str, char value);
/* Return pointer to the first occurrence in [str1] of any of the chars
 * that are part of [str2] */
char* strpbrk(const char* str1, const char* str2);
/* Return the length of the initial part of [str1] not containing any of
 * the characters that are part of [str2] */
size_t strcspn(const char* str1, const char* str2);
/* Return the length of the initial portion of [str1] containing only
 * characters that appear in [str2] */
size_t strspn(const char* str1, const char* str2);
/* Return pointer to the first occurrence of [str2] in [str1] */
char* strstr(const char* str1, const char* str2);
/* Split [str] into tokens using [delimiters] as delimiters */
char* strtok(char* str, const char* delimiters);

/* Return length of [str] */
size_t strlen(const char* str);

#endif /* H_STRING */
