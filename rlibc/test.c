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

#define puts(msg) write(STDOUT_FILENO, msg, strlen(msg))
#define putc(chr) write(STDOUT_FILENO, &chr, 1);
#define putsfd(fd, msg) write(fd, msg, strlen(msg));
#define putcfd(fd, chr) write(fd, &chr, 1);

extern char** environ;

int main()
{
    printf("Hello from printf! Here should be pi: %f\n", 3.1415927f);
    
	pid_t pid = fork();
	
	if (pid == 0)
	{
        printf("child: %i\n", pid);

        char ** argv = sbrk(128);
        argv[0] = sbrk(16);
        argv[1] = NULL;
        strcpy(argv[0], "world!");

        environ = NULL;
        
        execve("hellow", argv, environ);

		return 2;
	}
	else
	{
		printf("parent: %i\n", pid);
		
		

		wait(NULL);
		return 1;
	}
}
