/* This file is a part of the RazOS project
 *
 * POSIX and standard C type definitions
 *
 * Razbit 2014, 2016 */

#ifndef TYPES_H
#define TYPES_H

/* Bochs magic breakpoint. This is a rather odd place to
 * define this, but this header is included in nearly every file.. */
#define BOCHS_BREAK __asm__ __volatile__("xchgw %%bx, %%bx":::"memory")

/* These are used internally in this header */
typedef unsigned int __uint32_t;
typedef int __int32_t;
typedef unsigned short __uint16_t;
typedef short __int16_t;
typedef unsigned char __uint8_t;
typedef char __int8_t;

#endif /* TYPES_H */

/* When building the kernel or this header, enable all types by
 * passing '-D _RAZOS_KERNEL' or '-D _NEED_ALL_TYPES' to gcc */

#if defined(_RAZOS_KERNEL) || defined(_NEED_ALL_TYPES)

/* From stddef.h */
#define _NEED_NULL
#define _NEED_PTRDIFF_T
#define _NEED_SIZE_T
#define _NEED_WCHAR_T

/* From stdbool.h */
#define _NEED_BOOL

/* From stdint.h */
#define _NEED_STDINT
#define _NEED_INTPTR_T
#define _NEED_UINTPTR_T

/* From stdarg.h */
#define _NEED_VA_LIST

/* From all around POSIX */
#define _NEED_SSIZE_T
#define _NEED_OFF_T
#define _NEED_GID_T
#define _NEED_UID_T
#define _NEED_INO_T
#define _NEED_MODE_T
#define _NEED_PID_T
#define _NEED_DEV_T
#define _NEED_CLOCK_T
#define _NEED_TIME_T

#endif /* defined(_RAZOS_KERNEL) || defined(_NEED_ALL_TYPES) */


/* stdint */
#ifdef _NEED_STDINT
#define uint32_t __uint32_t
#define int32_t __int32_t
#define uint16_t __uint16_t
#define int16_t __int16_t
#define uint8_t __uint8_t
#define int8_t __int8_t
#undef _NEED_STDINT
#endif /* _NEED_STDINT */


/* Signed type for pointer subtraction */
#ifdef _NEED_PTRDIFF_T
#ifndef PTRDIFF_T
#define PTRDIFF_T
typedef __int32_t ptrdiff_t;
#endif /* PTRDIFF_T */
#undef _NEED_PTRDIFF_T
#endif /* _NEED_PTRDIFF_T */


/* intptr_t */
#ifdef _NEED_INTPTR_T
#ifndef INTPTR_T
#define INTPTR_T
typedef __int32_t intptr_t;
#endif /* INTPTR_T */
#undef _NEED_INTPTR_T
#endif /* _NEED_INTPTR_T */


/* uintptr_t */
#ifdef _NEED_UINTPTR_T
#ifndef UINTPTR_T
#define UINTPTR_T
typedef __uint32_t uintptr_t;
#endif /* UINTPTR_T */
#undef _NEED_UINTPTR_T
#endif /* _NEED_UINTPTR_T */


/* wchar_t */
#ifdef _NEED_WCHAR_T
#ifndef WCHAR_T
#define WCHAR_T
typedef __int32_t wchar_t;
#endif /* WCHAR_T */
#undef _NEED_WCHAR_T
#endif /* _NEED_WCHAR_T */


/* Bool */
#ifdef _NEED_BOOL
#define bool _Bool;
#define true 1
#define false 0
#define __bool_true_false_are_defined 1
#undef _NEED_BOOL
#endif /* _NEED_BOOL */


/* Null pointer */
#ifdef _NEED_NULL
#ifdef NULL
#undef NULL
#endif
#define NULL ((void *) 0)
#undef _NEED_NULL
#endif /* _NEED_NULL */


/* Unsigned type for size of something */
#ifdef _NEED_SIZE_T
#ifndef SIZE_T
#define SIZE_T
typedef __uint32_t size_t;
#endif /* SIZE_T */
#undef _NEED_SIZE_T
#endif /* _NEED_SIZE_T */


/* Signed size */
#ifdef _NEED_SSIZE_T
#ifndef SSIZE_T
#define SSIZE_T
typedef __int32_t ssize_t;
#endif /* SSIZE_T */
#undef _NEED_SSIZE_T
#endif /* _NEED_SSIZE_T */


/* Offset */
#ifdef _NEED_OFF_T
#ifndef OFF_T
#define OFF_T
typedef __int32_t off_t;
#endif /* OFF_T */
#undef _NEED_OFF_T
#endif /* _NEED_OFF_T */


/* Group ID */
#ifdef _NEED_GID_T
#ifndef GID_T
#define GID_T
typedef __uint32_t gid_t;
#endif /* GID_T */
#undef _NEED_GID_T
#endif /* _NEED_GID_T */


/* User ID */
#ifdef _NEED_UID_T
#ifndef UID_T
#define UID_T
typedef __uint32_t uid_t;
#endif /* UID_T */
#undef _NEED_UID_T
#endif /* _NEED_UID_T */


/* File serial number */
#ifdef _NEED_INO_T
#ifndef INO_T
#define INO_T
typedef __uint32_t ino_t;
#endif /* INO_T */
#undef _NEED_INO_T
#endif /* NEED_INO_T */


/* File mode (access, type..) */
#ifdef _NEED_MODE_T
#ifndef MODE_T
#define MODE_T
typedef __uint32_t mode_t;
#endif /* MODE_T */
#undef _NEED_MODE_T
#endif /* _NEED_MODE_T */


/* Device ID */
#ifdef _NEED_DEV_T
#ifndef DEV_T
#define DEV_T
typedef __uint32_t dev_t;
#endif /* DEV_T */
#undef _NEED_DEV_T
#endif /* _NEED_DEV_T */


/*  Process ID */
#ifdef _NEED_PID_T
#ifndef PID_T
#define PID_T
typedef __uint32_t pid_t;
#endif /* PID_T */
#undef _NEED_PID_T
#endif /* _NEED_PID_T */

/* va_list */
#ifdef _NEED_VA_LIST
#ifndef VA_LIST
#define VA_LIST
typedef __builtin_va_list va_list;
#endif /* VA_LIST */
#undef _NEED_VA_LIST
#endif /* _NEED_VA_LIST */

#ifdef _NEED_CLOCK_T
#ifndef CLOCK_T
#define CLOCK_T
typedef __uint32_t clock_t;
#endif /* CLOCK_T */
#undef _NEED_CLOCK_T
#endif /* _NEED_CLOCK_T */

#ifdef _NEED_TIME_T
#ifndef TIME_T
#define TIME_T
typedef __int32_t time_t;
#endif /* TIME_T */
#undef _NEED_TIME_T
#endif /* _NEED_TIME_T */
