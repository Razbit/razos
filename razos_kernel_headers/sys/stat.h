/* sys/stat.h -- file status information */

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

/* fcntl needs mode_t values, this keeps the rest of this header invisible */
#ifndef _NEED_MODE_T_VALUES

#ifndef STAT_H /* Include guard */
#define STAT_H

#define _NEED_DEV_T
#define _NEED_INO_T
#define _NEED_MODE_T
#define _NEED_UID_T
#define _NEED_GID_T
#define _NEED_OFF_T
#define _NEED_TIME_T

#include <sys/types.h>

/* A partial POSIX stat -structure. Members will be added
 * when the kernel supports them. */
struct stat
{
	dev_t st_dev;     /* Device ID of device containing the file */
	ino_t st_ino;     /* File serial number */
	mode_t st_mode;   /* File mode: access, type */
	uid_t st_uid;     /* User ID of file */
	gid_t st_gid;     /* Group ID of file */
	dev_t st_rdev;    /* Device ID if file is char/block special */
	off_t st_size;    /* Size of file */
	time_t st_atime;  /* Last accessed */
	time_t st_mtime;  /* Last modified */
	time_t st_ctime;  /* Status last changed */
};

/* Function prototypes */
/* Get file status information */
int stat(const char* path, struct stat* buf);
int fstat(int fd, struct stat* buf);

int mkfifo(const char* path, mode_t mode);

#endif /* STAT_H */
#endif /* _NEED_MODE_T_VALUES */

/* Values for mode_t */
/* File type */
#define S_IFMT   0x00FF0000 /* Mask of file type info */

#define S_IFBLK  0x00010000 /* Block device */
#define S_IFCHR  0x00020000 /* Character device */
#define S_IFIFO  0x00040000 /* FIFO */
#define S_IFREG  0x00080000 /* Regular file */
#define S_IFDIR  0x00100000 /* Directory */
#define S_IFLNK  0x00200000 /* Symbolic link */
#define S_IFSOCK 0x00400000 /* Socket */

/* File mode */
#define S_IPERM  07777 /* Permission mask */

#define S_IRUSR  00400 /* User read perm */
#define S_IWUSR  00200 /* User write perm */
#define S_IXUSR  00100 /* User exec perm */

#define S_IRGRP  00040 /* Group read perm */
#define S_IWGRP  00020 /* Group write perm */
#define S_IXGRP  00010 /* Group exec perm */

#define S_IROTH  00004 /* Others read perm */
#define S_IWOTH  00002 /* Others write perm */
#define S_IXOTH  00001 /* Others exec perm */

#define S_IRWXU  (S_IRUSR | S_IWUSR | S_IXUSR)
#define S_IRWXG  (S_IRGRP | S_IWGRP | S_IXGRP)
#define S_IRWXO  (S_IROTH | S_IWOTH | S_IXOTH)

#define S_ISUID  04000 /* Set UID at execution */
#define S_ISGID  02000 /* Set GID at execution */
#define S_ISVTX  01000 /* Sticky bit: deletion and renaming only
                             * by file owner, dir owner or root */

/* Test macros for mode_t */
#define S_ISBLK(m)  (((m) & S_IFBLK) ? 1 : 0)
#define S_ISCHR(m)  (((m) & S_IFCHR) ? 1 : 0)
#define S_ISDIR(m)  (((m) & S_IFDIR) ? 1 : 0)
#define S_ISFIFO(m) (((m) & S_IFIFO) ? 1 : 0)
#define S_ISREG(m)  (((m) & S_IFREG) ? 1 : 0)
#define S_ISLNK(m)  (((m) & S_IFLNK) ? 1 : 0)
#define S_ISSOCK(m) (((m) & S_IFSOCK) ? 1 : 0)
