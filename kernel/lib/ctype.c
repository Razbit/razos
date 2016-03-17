/* Copyright (c) 2014 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

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
