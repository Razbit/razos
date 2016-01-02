#include <api/razos.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sched.h>

int main()
{
	char str[] = "Hello FS from user-space!\n\0";
	write(STDOUT_FILENO, &(str[0]) , strlen(str));

	pid_t pid = fork();
	if (pid == 0)
	{
		write(STDOUT_FILENO, "child: ", strlen("child: "));
		char c = pid+0x30;
		write(STDOUT_FILENO, &c, 1);
		c = '\n';
		write(STDOUT_FILENO, &c, 1);
		sched_yield();
		write(STDOUT_FILENO, "yielded", strlen("yielded"));
		return 2;
	}
	else
	{
		write(STDOUT_FILENO, "parent: ", strlen("parent: "));
		char c = pid+0x30;
		write(STDOUT_FILENO, &c, 1);
		c = '\n';
		write(STDOUT_FILENO, &c, 1);

		wait(NULL);
		return 1;
	}
}
