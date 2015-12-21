#include <api/razos.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char str[] = "Hello FS from user-space!\n\0";
	write(STDOUT_FILENO, &(str[0]) , strlen(str));

	char buf[10];
	read(STDIN_FILENO, &(buf[0]), 2);
	write(STDIN_FILENO, "\n", 1);
	write(STDOUT_FILENO, &(buf[0]), 2);

	return 1;
}
