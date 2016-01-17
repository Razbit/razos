/* This file is a part of the RazOS project.
 *
 * Razbit 2014 */

#include <ctype.h>

int isalnum(int c)
{
	if ((c >= 0x41 && c <=0x5A)					\
	    || (c >= 0x61 && c <= 0x7A)				\
	    || (c >= 0x30 && c <= 0x39))
		return 1;
	return 0;
}

int isalpha(int c)
{
	if ((c >= 0x41 && c <=0x5A) || (c >= 0x61 && c <= 0x7A))
		return 1;
	return 0;
}

int isdigit(int c)
{
	if (c >= 0x30 && c <= 0x39)
		return 1;
	return 0;
}

int iscntrl(int c)
{
	if (c <= 0x1F || c == 0x7F)
		return 1;
	return 0;
}

int isgrapf(int c)
{
	if (c >= 0x21 && c <= 0x7E)
		return 1;
	return 0;
}

int islower(int c)
{
	if (c >= 0x61 && c <= 0x7A)
		return 1;
	return 0;
}

int isprint(int c)
{
	if (c >= 0x20 && c <= 0x7E)
		return 1;
	return 0;
}

int ispunct(int c)
{
	if ((c >= 0x21 && c <= 0x2F)				\
	    || (c >= 0x3A && c <= 0x40)				\
	    || (c >= 0x5B && c <= 0x60)				\
	    || (c >= 0x7B && c <= 0x7E))
		return 1;
	return 0;
}

int isspace(int c)
{
	if ((c >= 0x09 && c <= 0x0D) || c == 0x20)
		return 1;
	return 0;
}

int isupper(int c)
{
	if (c >= 0x41 && c <= 0x5A)
		return 1;
	return 0;
}

int isxdigit(int c)
{
	if ((c >= 0x30 && c <= 0x39)				\
	    || (c >= 0x41 && c <= 0x46)				\
	    || (c >= 0x61 && c <= 0x66))
		return 1;
	return 0;
}

int tolower(int c)
{
	if (isupper(c))
		c += 0x20;
	return c;
}

int toupper(int c)
{
	if (islower(c))
		c -= 0x20;
	return c;
}
