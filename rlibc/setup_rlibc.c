/* setup_rlibc.h -- setup the library */

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

#include <api/razos.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

extern char** environ;

/* Set kernel parameter */
int set_kernel_param(unsigned int cmd, int param)
{
	return (int)__syscall2(SYS_SETUP, (uint32_t)cmd, (uint32_t)param);
}

/* Initialize the lib. Called from crt0.s _start */
void init_rlibc(int argc, char* argv[], char* envp[])
{
	environ = NULL;
	/* Set environ using setenv() */
	if (envp != NULL)
	{
		size_t i = 0;
		char* name;
		char* val;
		while (envp[i] != NULL)
		{
			name = envp[i];
			val = strchr(envp[i], '=');
			*val = '\0';
			setenv(name, ++val, 1);
			i++;
		}
	}

	int* errno_loc = __errno_location();
	set_kernel_param(SET_ERRNO_LOC, (int)errno_loc);

	/* Without these GCC breaks the stack.. */
	__asm__ __volatile__("mov %0, %%esi" :: "r"(envp) : "memory");
	__asm__ __volatile__("mov %0, %%edi" :: "r"(argv) : "memory");
	__asm__ __volatile__("mov %0, %%eax" :: "r"(argc) : "memory");
}
