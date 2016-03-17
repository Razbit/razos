/* Copyright (c) 2015 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#ifndef H_STRING
#define H_STRING

#define _NEED_NULL
#define _NEED_SIZE_T
#include <sys/types.h>

/* Set [num] bytes of memory starting from [ptr] to [value] */
void* memset(void* ptr, int value, int num);
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
void* memchr(const void* ptr, int value, size_t num);
/* Return pointer to first occurence of [value] in the string [str] */
char* strchr(const char* str, int value);
/* Return pointer to last occurence of [value] in the string [str] */
char* strrchr(const char* str, int value);
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

/* Get error message string */
char* strerror(int err);

#endif /* H_STRING */
