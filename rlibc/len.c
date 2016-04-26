#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	char *line;
	size_t n;
	dgetline(&line, &n, STDIN_FILENO);
	printf("%d\n", strlen(line));
	return 0;
}
