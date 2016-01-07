#include <api/razos.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sched.h>
#include <sys/stat.h>

#define puts(msg) write(STDOUT_FILENO, msg, strlen(msg))
#define putc(chr) write(STDOUT_FILENO, &chr, 1);

int main()
{
	char str[] = "Hello FS from user-space!\n";
	puts(str);
    	
	pid_t pid = fork();
	if (pid == 0)
	{
		puts("child: ");
		char c = pid+0x30;
		putc(c);
		puts("\n");

		return 2;
	}
	else
	{
		puts("parent: ");
		char c = pid+0x30;
		putc(c);
		puts("\n");
		
		wait(NULL);
		return 1;
	}
}
