/* dirent.h - directory entries */

/* Copyright (c) 2017 Eetu "Razbit" Pesonen
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

#ifndef DIRENT_H
#define DIRENT_H

#define _NEED_INO_T
#include <sys/types.h>

/* A type representing a directory stream */
typedef int DIR;

struct dirent
{
	ino_t d_ino;  /* Directory serial number */
	char *d_name; /* Entry name */
};


int closedir(DIR* dirp);
DIR* opendir(const char* path);
struct dirent* readdir(DIR* dirp);
void rewinddir(DIR* dirp);

#endif /* DIRENT_H */
