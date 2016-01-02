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
		write(STDOUT_FILENO, "child\n", strlen("child\n"));
	}
	else
	{
		write(STDOUT_FILENO, "parent\n", strlen("parent\n"));
	}

	for(;;);
	return 1;
}
