#include <stdio.h>

#define puts(msg) write(STDOUT_FILENO, msg, strlen(msg))

int main()
{
	char str[] = "World";
	printf("Hello, %s!\n", str);
	return 0;
}
