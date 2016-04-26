#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>

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

		if (c == 127 || c == 8)
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

static char read_char(void)
{
	int c;
	int position = 0;

	for (;;)
	{
		c = getchar();
		if (c == 127 || c == 8)
		{
			if (position > 0)
			{
				printf("\b \b");
				--position;
			}
		}
		else
		{
			if (c == '\n')
			{
				puts("");
				position = 0;
			}
			else
			{
				printf("%c", c);
				++position;
			}
			return c;
		}
	}
}

int main(int argc, char* argv[])
{
	int fd[2];
	int childpid;
	char c;
	int status;
	int flags;

	pipe(fd);
	if ((childpid = fork()) < 0)
	{
		exit(EXIT_FAILURE);
	}
	// Child closes output, parent closes input.
	if (childpid == 0)
	{
		close(0);
		close(fd[1]);
		dup(fd[0]);
		execv("rash", 0);
	}
	close(fd[0]);

	flags = fcntl(0, F_GETFL, 0);
	fcntl(0, F_SETFL, flags | O_NONBLOCK);
	for (;;)
	{
		c = read_char();
		write(fd[1], &c, 1);
		if (c == EOF)
		{
			break;
		}
	}
	wait(&status);

	return 0;
}
