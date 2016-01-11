#include <api/razos.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sched.h>
#include <sys/stat.h>
#include <errno.h>

#define puts(msg) write(STDOUT_FILENO, msg, strlen(msg))
#define putc(chr) write(STDOUT_FILENO, &chr, 1);
#define putsfd(fd, msg) write(fd, msg, strlen(msg));
#define putcfd(fd, chr) write(fd, &chr, 1);


int main()
{
	puts("Hello FS from user-space!\n");

	int pipefd[2];
	pipe(pipefd);
    	
	pid_t pid = fork();
	if (pid == 0)
	{
		puts("child: ");
		char c = pid+0x30;
		putc(c);
		puts("\n");

		close(pipefd[0]);

		putsfd(pipefd[1], "Hello, piped world!\n")

		sched_yield();

		close(pipefd[1]);
		return 2;
	}
	else
	{
		puts("parent: ");
		char c = pid+0x30;
		putc(c);
		puts("\n");
		
		close(pipefd[1]);

		char buf[32];
		read(pipefd[0], buf, 32);
		puts(buf);

		wait(NULL);

		close(pipefd[0]);
		return 1;
	}
}
