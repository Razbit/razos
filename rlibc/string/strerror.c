/* This file is a part of the RazOS project
 *
 * strerror.c -- strerror(): get error message string
 *
 * Razbit 2016 */

#include <string.h>
#include <errno.h>

const char* str[] =
{
	[EDOM] = "Argument out of function domain",
	[ERANGE] = "Result out of range",
	[EILSEQ] = "Illegal byte sequence",
	[E2BIG] = "Argument list too long",
	[EACCES] = "Permission denied",
	[EADDRINUSE] = "Address in use",
	[EADDRNOTAVAIL] = "Address not available",
	[EAFNOSUPPORT] = "Address family not supported",
	[EAGAIN] = "Resource unavailable, try again",
	[EALREADY] = "Connection already in progress",
	[EBADF] = "Bad file descriptor",
	[EBADMSG] = "Bad message",
	[EBUSY] = "Device or resource busy",
	[ECANCELED] = "Operation canceled",
	[ECHILD] = "No child processes",
	[ECONNABORTED] = "Connection aborted",
	[ECONNREFUSED] = "Connection refused",
	[ECONNRESET] = "Connection reset",
	[EDEADLK] = "Resource deadlock would occur",
	[EDESTADDRREQ] = "Destination address required",
	[EDQUOT] = "Reserved",
	[EEXIST] = "File exists",
	[EFAULT] = "Bad address",
	[EFBIG] = "File too large",
	[EHOSTUNREACH] = "Host is unreachable",
	[EIDRM] = "Identifier removed",
	[EINPROGRESS] = "Operation in progress",
	[EINTR] = "Interrupted function",
	[EINVAL] = "Invalid argument",
	[EIO] = "I/O error",
	[EISCONN] = "Socket is connected",
	[EISDIR] = "Is a directory",
	[ELOOP] = "Too many levels of symbolic links",
	[EMFILE] = "File descriptor value too large",
	[EMLINK] = "Too many links",
	[EMSGSIZE] = "Message too large",
	[EMULTIHOP] = "Reserved",
	[ENAMETOOLONG] = "Filename too long",
	[ENETDOWN] = "Network is down",
	[ENETRESET] = "Connection aborted by network",
	[ENETUNREACH] = "Network unreachable",
	[ENFILE] = "Too many files open in system",
	[ENOBUFS] = "No buffer space available",
	[ENODATA] = "No data available",
	[ENODEV] = "No such device",
	[ENOENT] = "No such file or directory",
	[ENOEXEC] = "Executable file format error",
	[ENOLCK] = "No locks available",
	[ENOLINK] = "Reserved",
	[ENOMEM] = "Not enough space",
	[ENOMSG] = "No message of the #define esired type",
	[ENOPROTOOPT] = "Protocol not available",
	[ENOSPC] = "No space left on device",
	[ENOSR] = "Out of streams resources",
	[ENOSTR] = "Device not a stream",
	[ENOSYS] = "Function not supported",
	[ENOTCONN] = "The socket is not connected",
	[ENOTDIR] = "Not a directory or a symlink to a directory",
	[ENOTEMPTY] = "Directory not empty",
	[ENOTRECOVERABLE] = "State not recoverable",
	[ENOTSOCK] = "Not a socket",
	[ENOTSUP] = "Not supported",
	[ENOTTY] = "Inappropriate I/O control operation",
	[ENXIO] = "No such device or address",
	[EOPNOTSUPP] = "Operation not supported on socket",
	[EOVERFLOW] = "Value too large to be stored in data type",
	[EOWNERDEAD] = "Previous owner died",
	[EPERM] = "Operation not permitted",
	[EPIPE] = "Broken pipe",
	[EPROTO] = "Protocol error",
	[EPROTONOSUPPORT] = "Protocol not supported",
	[EPROTOTYPE] = "Protocol wrong type for socket",
	[EROFS] = "Read-only file system",
	[ESPIPE] = "Invalid seek",
	[ESRCH] = "No such process",
	[ESTALE] = "Reserved",
	[ETIME] = "Timer expired",
	[ETIMEDOUT] = "Connection timed out",
	[ETXTBSY] = "Text file busy",
	[EWOULDBLOCK] = "Operation would block",
	[EXDEV] = "Cross-device link"
};

char* strerror(int err)
{
	return str[err];
}
