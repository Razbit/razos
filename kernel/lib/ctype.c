/* This file is a part of the RazOS project.
 *
 * Razbit 2014 */

#include <ctype.h>

/* Take a look at an ASCII chart */

int isalnum(char c)
{
	if (isalpha(c) || isdigit(c))
		return 1;
	return 0;
}

int isalpha(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return 1;
	return 0;
}

int isdigit(char c)
{
	if (c >= '0' && c <= '9')
		return 1;
	return 0;
}

int iscntrl(char c)
{
	if (c <= 0x1F || c == 0x7F)
		return 1;
	return 0;
}

int isgrapf(char c)
{
	if (c >= 0x21 && c <= 0x7E)
		return 1;
	return 0;
}

int islower(char c)
{
	if (c >= 'a' && c <= 'z')
		return 1;
	return 0;
}

int isprint(char c)
{
	if (c >= 0x20 && c <= 0x7E)
		return 1;
	return 0;
}

int ispunct(char c)
{
	if ((c >= 0x21 && c <= 0x2F)				\
	    || (c >= 0x3A && c <= 0x40)				\
	    || (c >= 0x5B && c <= 0x60)				\
	    || (c >= 0x7B && c <= 0x7E))
		return 1;
	return 0;
}

int isspace(char c)
{
	if ((c >= 0x09 && c <= 0x0D) || c == 0x20)
		return 1;
	return 0;
}

int isupper(char c)
{
	if (c >= 'A' && c <= 'Z')
		return 1;
	return 0;
}

int isxdigit(char c)
{
	if (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
		return 1;
	return 0;
}

int tolower(char c)
{
	if (isupper(c))
		c += 0x20;
	return c;
}

int toupper(char c)
{
	if (islower(c))
		c -= 0x20;
	return c;
}
