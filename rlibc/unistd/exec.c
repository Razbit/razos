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

#include <unistd.h>
#include <api/razos.h>

extern char** environ;

int execve(const char* path, char* const argv[], char* const envp[])
{
	return __syscall3(SYS_EXECVE, (uint32_t)path, (uint32_t)argv, \
	                  (uint32_t)envp);
}

int execv(const char* path, char* const argv[])
{
	return execve(path, argv, environ);
}
