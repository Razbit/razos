#include <api/razos.h>

void console_log(const char* str)
{
	uint32_t len = 0;
	for (const char* ptr = str; *ptr; ptr++)
		len++;

	__syscall2(SYS_CONSOLE_LOG, (uint32_t)str, len);
}

__attribute__ ((__noreturn__)) void _exit(uint32_t status)
{
	__syscall1(SYS_EXIT, status);
}

uint32_t main()
{
	console_log("Hello from Userworld!\n");
	return 1;
}
