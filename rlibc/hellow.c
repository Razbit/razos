#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

int main(int argc, char* argv[])
{
	printf("%u KiB memory in use\n", get_used_mem()/1024);

	printf("hellow running in %s\n", getenv("SHELL"));
	if (argc > 1)
		printf("Hello, %s\n", argv[1]); /* argv[0] == program name */
	else
		printf("Hellow: no arguments given\n");
	return 0;
}
