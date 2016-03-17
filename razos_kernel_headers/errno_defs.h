/* errno_defs.h -- define errno numbers */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
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

#ifndef H_ERRNO_DEFS
#define H_ERRNO_DEFS

/* ISO C99 */
#define EDOM             1  /* Mathematics argument out of domain */
#define ERANGE           2  /* Result out of range. */
#define EILSEQ           3  /* Illegal byte sequence. */

/* From POSIX.1-2008 */
#define E2BIG            4  /* Argument list too long. */
#define EACCES           5  /* Permission denied. */
#define EADDRINUSE       6  /* Address in use. */
#define EADDRNOTAVAIL    7  /* Address not available. */
#define EAFNOSUPPORT     8  /* Address family not supported. */
#define EAGAIN           9  /* Resource unavailable, try again. */
#define EALREADY        10  /* Connection already in progress. */
#define EBADF           11  /* Bad file descriptor. */
#define EBADMSG         12  /* Bad message. */
#define EBUSY           13  /* Device or resource busy. */
#define ECANCELED       14  /* Operation canceled. */
#define ECHILD          15  /* No child processes. */
#define ECONNABORTED    16  /* Connection aborted. */
#define ECONNREFUSED    17  /* Connection refused. */
#define ECONNRESET      18  /* Connection reset. */
#define EDEADLK         19  /* Resource deadlock would occur. */
#define EDESTADDRREQ    20  /* Destination address required. */
#define EDQUOT          21  /* Reserved. */
#define EEXIST          22  /* File exists. */
#define EFAULT          23  /* Bad address. */
#define EFBIG           24  /* File too large. */
#define EHOSTUNREACH    25  /* Host is unreachable. */
#define EIDRM           26  /* Identifier removed. */
#define EINPROGRESS     27  /* Operation in progress. */
#define EINTR           28  /* Interrupted function. */
#define EINVAL          29  /* Invalid argument. */
#define EIO             30  /* I/O error. */
#define EISCONN         31  /* Socket is connected. */
#define EISDIR          32  /* Is a directory. */
#define ELOOP           33  /* Too many levels of symbolic links. */
#define EMFILE          34  /* File descriptor value too large. */
#define EMLINK          35  /* Too many links. */
#define EMSGSIZE        36  /* Message too large. */
#define EMULTIHOP       37  /* Reserved. */
#define ENAMETOOLONG    38  /* Filename too long. */
#define ENETDOWN        39  /* Network is down. */
#define ENETRESET       40  /* Connection aborted by network. */
#define ENETUNREACH     41  /* Network unreachable. */
#define ENFILE          42  /* Too many files open in system. */
#define ENOBUFS         43  /* No buffer space available. */
#define ENODATA         44  /* No data available. */
#define ENODEV          45  /* No such device. */
#define ENOENT          46  /* No such file or directory. */
#define ENOEXEC         47  /* Executable file format error. */
#define ENOLCK          48  /* No locks available. */
#define ENOLINK         49  /* Reserved. */
#define ENOMEM          50  /* Not enough space. */
#define ENOMSG          51  /* No message of the desired type. */
#define ENOPROTOOPT     52  /* Protocol not available. */
#define ENOSPC          53  /* No space left on device. */
#define ENOSR           54  /* Out of streams resources. */
#define ENOSTR          55  /* Device not a stream. */
#define ENOSYS          56  /* Function not supported. */
#define ENOTCONN        57  /* The socket is not connected. */
#define ENOTDIR         58  /* Not a directory or a symlink to one. */
#define ENOTEMPTY       59  /* Directory not empty. */
#define ENOTRECOVERABLE 60  /* State not recoverable. */
#define ENOTSOCK        61  /* Not a socket. */
#define ENOTSUP         62  /* Not supported. */
#define ENOTTY          63  /* Inappropriate I/O control operation. */
#define ENXIO           64  /* No such device or address. */
#define EOPNOTSUPP      65  /* Operation not supported on socket. */
#define EOVERFLOW       66  /* Value too large to be stored in type. */
#define EOWNERDEAD      67  /* Previous owner died. */
#define EPERM           68  /* Operation not permitted. */
#define EPIPE           69  /* Broken pipe. */
#define EPROTO          70  /* Protocol error. */
#define EPROTONOSUPPORT 71  /* Protocol not supported. */
#define EPROTOTYPE      72  /* Protocol wrong type for socket. */
#define EROFS           73  /* Read-only file system. */
#define ESPIPE          74  /* Invalid seek. */
#define ESRCH           75  /* No such process. */
#define ESTALE          76  /* Reserved. */
#define ETIME           77  /* Timer expired. */
#define ETIMEDOUT       78  /* Connection timed out. */
#define ETXTBSY         79  /* Text file busy. */
#define EWOULDBLOCK     80  /* Operation would block. */
#define EXDEV           81  /* Cross-device link. */

#endif /* H_ERRNO_DEFS */
