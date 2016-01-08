/* This file is a part of the RazOS project
 *
 * errno.h -- errno
 *
 * Razbit 2016 */

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
