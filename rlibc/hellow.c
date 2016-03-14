#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	printf("hellow running in %s\n", getenv("SHELL"));
	if (argc > 1)
		printf("Hello, %s\n", argv[1]); /* argv[0] == program name */
	else
		printf("Hellow: no arguments given\n");
	return 0;
}
