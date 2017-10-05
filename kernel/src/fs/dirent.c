/* dirent.c - directory entries */

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

#include <dirent.h>
#include <string.h>
#include <kmalloc.h>

#include "vfs.h"
#include "../mm/task.h"


int closedir(DIR* dirp)
{

	return 0;
}


DIR* opendir(const char* path)
{

}


struct dirent* readdir(DIR* dirp)
{

}


/* reset the position of a directory stream to the
 * beginning of a directory */
void rewinddir(DIR* dirp)
{

}
