#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	printf("hellow running in %s\n", getenv("SHELL"));
	printf("Hello, %s\n", argv[0]);
	return 0;
}
