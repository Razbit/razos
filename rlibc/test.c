#include <api/razos.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char str[] = "Hello FS from user-space!\n\0";
	write(STDOUT_FILENO, &(str[0]) , strlen(str));
	return 1;
}
