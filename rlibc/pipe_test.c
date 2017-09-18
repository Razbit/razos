#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;

    if (argc < 2)
	    return -1;

    if (pipe(pipefd) == -1)
    {
	    printf("pipe");
	    return -1;
    }

    cpid = fork();
    if (cpid == -1)
    {
	    printf("fork");
	    return -1;
    }

    if (cpid == 0)
    {
	    /* Child reads from pipe */
	    /* Close unused write end */
	    close(pipefd[1]);

	    while (read(pipefd[0], &buf, 1) > 0)
        {
	        printf("%c", buf);
        }

        printf("\n");
        close(pipefd[0]);
        return 0;
    }
    else
    {
	    /* Parent writes  to pipe */
        close(pipefd[0]);          /* Close unused read end */
        write(pipefd[1], argv[1], strlen(argv[1]));
        close(pipefd[1]);          /* Reader will see EOF */
                wait(NULL);              /* Wait for child */
        return 0;
    }
}
