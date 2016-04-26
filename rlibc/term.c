#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

/* Read a line from stdin and return it. */
static char *read_line(void)
{
	int bufsize = 80;
	int position = 0;
	char *buffer = malloc(bufsize * sizeof(char));
	int c;

	if (buffer == NULL)
	{
		exit(EXIT_FAILURE);
	}

	for (;;)
	{
		c = getchar();

		if (c == 127)
		{
			if (position > 0)
			{
				printf("\b \b");
				--position;
			}
		}
		else if (c == EOF || c == '\n')
		{
			puts("");
			buffer[position] = '\0';
			return buffer;
		}
		else
		{
			printf("%c", c);
			buffer[position] = c;
			++position;
		}

		/* If the buffer is exceeded, reallocate. */
		if (position >= bufsize)
		{
			bufsize *= 2; /* Double the buffer size. */
			buffer = realloc(buffer, bufsize * sizeof(char));
			if (buffer == NULL)
			{
				exit(EXIT_FAILURE);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	int fd[2];
	int childpid;
	int status;
	char *line;
	size_t n;

	pipe(fd);
	if ((childpid = fork()) == -1)
	{
		exit(EXIT_FAILURE);
	}
	// Child closes output, parent closes input.
	if (childpid == 0)
	{
		close(0);
		close(fd[1]);
		dup(fd[0]);
		//execv("len", NULL);
		dgetline(&line, &n, 0);
		printf("From child: %s\n", line);
		exit(0);
	}
	close(fd[0]);

	puts("Write a line from parent:");
	line = read_line();
	write(fd[1], line, strlen(line)+1);
	write(fd[1], "\n", 1);
	wait(&status);

	return 0;
}
