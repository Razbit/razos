/* This file is a part of the RazOS project
 *
 * stdio.h -- C99-ish stdio
 *
 * Razbit 2016 */

#ifndef STDIO_H
#define STDIO_H

/* Types */
#define _NEED_VA_LIST
#define _NEED_SIZE_T
#define _NEED_SSIZE_T
#define _NEED_NULL
#include <sys/types.h>

#define _NEED_WHENCE_VALUES
#include <unistd.h>
#undef _NEED_WHENCE_VALUES


/* Function prototypes */

/* printf -family */
int fprintf(FILE* stream, const char* fmt, ...);
int vfprintf(FILE* stream, const char* fmt, va_list arg);

int printf(const char* fmt, ...);
int vprintf(const char* fmt, va_list arg);

int snprintf(char* buf, size_t num, const char* fmt, ...);
int vsnprintf(char* buf, size_t num, const char* fmt, va_list arg);

int sprintf(char* buf, const char* fmt, ...);
int vsprintf(char* buf, const char* fmt, va_list arg);

/* ISO C99 extenstions to printf family from POSIX 2008. These take integer
 * file descriptor instead of FILE*. Analogous to fprintf, vfprintf */
int dprintf(int fd, const char* fmt, ...);
int vdprintf(int fd, const char* fmt, va_list arg);


/* The put -family of functions */
int fputc(int chr, FILE* stream);
int fputs(const char* str, FILE* stream);

int putc(int chr, FILE* stream);
int puts(const char* str);

int putchar(int c);

/* rlibc extensions to C99 put -family. Analogous to fputc/fputs. These
 * take POSIX file descriptor instead of FILE stream */
int dputc(int chr, int fd);
int dputs(const char* str, int fd);


/* The get-family of functinos */
int fgetc(FILE* stream);
char* fgets(char* buf, int num, FILE* stream);

int getc(FILE* stream);
char* gets(char* buf);

int getchar();

ssize_t getdelim(char** lineptr, size_t* n, int delim, FILE* stream);
ssize_t getline(char** lineptr, size_t* n, FILE* stream);

/* rlibc extensions to C99 get -family. Analogous to fgetc/fgets/getdelim/getline.
 * These take POSIX file descriptor instead of FILE stream */
int dgetc(int fd);
int dgets(char* buf, int num, int fd);

ssize_t dgetdelim(char** lineptr, size_t* n, int delim, int fd);
ssize_t dgetdelim(char** lineptr, size_t* n, int fd);

#endif /* STDIO_H */
