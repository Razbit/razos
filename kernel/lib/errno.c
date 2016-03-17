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

#include <sys/types.h>
#include <errno.h>
#include <../src/mm/task.h>

int k_errno = 0;

int* __get_errno_loc()
{
	if (cur_task == NULL)
	{
		/* Use kernel errno */
		return &k_errno;
	}
	else
	{
		/* Use user-provided errno */
		return cur_task->errno_loc;
	}
}
