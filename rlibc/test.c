#include <api/razos.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sched.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

extern char** environ;

int main(int argc, char* argv[])
{
	time_t ut = time(NULL);
	struct tm time1 = *gmtime(&ut);    
	printf("Time since Epoch %i seconds\n", ut);    
	puts(asctime(&time1));
    
	pid_t pid = fork();
	
	if (pid == 0)
	{
		printf("child: %i\n", pid);

		char* argv2[] = {"world", NULL};
		setenv("SHELL", "bash", 1);
		execve("hellow", argv2, environ);

		return 2;
	}
	else
	{
		printf("parent: %i\n", pid);
		int ret;
		wait(&ret);
		printf("child returned %i\n", ret);
		return 1;
	}
}
