/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include <vsprintf.h>

/* printf flags */
#define FL_SIGN 0x01
#define FL_LEFT_JUSTIFY 0x02
#define FL_PLUS 0x04
#define FL_SPACE 0x08
#define FL_SPECIAL 0x10
#define FL_ZEROPAD 0x20
#define FL_SMALL 0x40

static int atoi_ptr(const char **str)
{
	int i = 0;

	while (isdigit(**str))
		i = i * 10 + *((*str)++) - '0';

	return i;
}

static char* numstr(char* str, long number, int base, int width, int prec, int flags)
{
/* Convert number [num] of radix [base] */
/* Not much to explain, just fancy pointer fiddling */

	char c, sign, tmp[36];
	const char* digs = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;
	uint32_t num;

	if (width < 0)
		width = -width;

	if (flags & FL_SMALL)
		digs = "0123456789abcdefghijklmnopqrstuvwxyz";

	if (base<2 || base>36)
		return 0;

	if (flags & FL_ZEROPAD)
		c = '0';
	else
		c = ' ';

	if (flags & FL_SIGN && number < 0)
	{
		sign = '-';
		num = -number;
	}
	else
	{
		num = number;
		if (flags & FL_PLUS)
			sign = '+';
		else
		{
			if (flags & FL_SPACE)
				sign = ' ';
			else
				sign = 0;
		}
	}

	if (sign != 0)
		width--;

	if (flags & FL_SPECIAL)
	{
		if (base == 16)
			width -= 2;
		else if (base == 8)
			width--;
	}

	i = 0;

	if (num == 0)
		tmp[i++]='0';
	else
	{
		while (num != 0)
		{
			tmp[i++] = digs[num % base];
			num /= base;
		}

	}

	if (i > prec)
		prec = i;

	width -= prec;

	if (!(flags & (FL_LEFT_JUSTIFY+FL_ZEROPAD)))
		while (width-- > 0)
			*str++ = ' ';

	if (sign)
		*str++ = sign;

	if (flags & FL_SPECIAL)
	{
		if (base == 8)
		{
			*str++ = '0';
		}
		else if (base == 16)
		{
			*str++ = '0';
			*str++ = digs[33]; /* x or X */
		}
	}

	if (!(flags&FL_LEFT_JUSTIFY))
		while(width-- > 0)
			*str++ = c;

	while (i < prec--)
		*str++ = '0';

	while (i-- > 0)
		*str++ = tmp[i];

	while (width-- > 0)
		*str++ = ' ';

	return str;
}

int vsprintf(char* buf, const char* fmt, va_list arg)
{
	uint8_t flags = 0;
	int state = 0;
	int width = -1;
	int prec = -1;
	int len = 0;
	int *printed;

	char *str = buf;
	char *s;
	long n;

	int slen;

	/* State-machine: Go through every char in fmt, do things with it */
	while (*fmt != '\0')
	{
		switch (state)
		{
		case 0: /* wait for % -char. Until found, keep printing */
			if(*fmt != '%')
			{
				*str++ = *fmt++;
				break;
			}
			state++;
			fmt++;

		case 1: /* % received; wait for flags */
			if (*fmt == '%') /* If we got another %, print it */
			{
				*str++ = *fmt++;
				state = 0;
				break;
			}

			if (*fmt == '-')
			{
				flags |= FL_LEFT_JUSTIFY;
				fmt++;
				break;
			}

			if (*fmt == '0')
			{
				flags |= FL_ZEROPAD;
				fmt++;
				break;
			}

			if (*fmt == '+')
			{
				flags |= FL_PLUS;
				flags |= FL_SIGN;
				fmt++;
				break;
			}

			if (*fmt == ' ')
			{
				flags |= FL_SPACE;
				fmt++;
				break;
			}

			if (*fmt == '#')
			{
				flags |= FL_SPECIAL;
				fmt++;
				break;
			}

			/* Take care of flag 'precedence' */
			if (flags & FL_PLUS)
				flags &= ~FL_SPACE; /* Remove the space flag */

			if (flags & FL_LEFT_JUSTIFY)
				flags &= ~FL_ZEROPAD;

			state++;

		case 2: /* Field width */
			if (isdigit(*fmt))
				width = atoi_ptr(&fmt);
			else if (*fmt == '*')
			{
				width = va_arg(arg, int);
				fmt++;
			}

			state++;

		case 3: /* precision */
			if (*fmt == '.')
			{
				fmt++;

				if (isdigit(*fmt))
					prec = atoi_ptr(&fmt);
				else if (*fmt == '*')
				{
					prec = va_arg(arg, int);
					fmt++;
				}

				state++;
			}
			else
			{
				state++;
				break;
			}

		case 4: /* length modifier */
			if (*fmt == 'l')
			{
				len = 1;
				fmt++;
			}
			else if (*fmt == 'h')
			{
				len = -1;
				fmt++;
			}

			state++;

		case 5: /* Conversion specifier */
			switch (*fmt)
			{
			case 'd':
			case 'i':
				flags |= FL_SIGN;
			case 'u':
				if (len == 0)
					n = va_arg(arg, int);
				else if (len == -1)
					n = (short)va_arg(arg, int);
				else
					n = va_arg(arg, long);

				str = numstr(str, n, 10, width, prec, flags);

				break;

			case 'o':
				if (len == 0)
					n = va_arg(arg, int);
				else if (len == -1)
					n = (short)va_arg(arg, int);
				else
					n = va_arg(arg, long);

				str = numstr(str, n, 8, width, prec, flags);

				break;

			case 'x':
				flags |= FL_SMALL;
			case 'X':
				if (len == 0)
					n = va_arg(arg, int);
				else if (len == -1)
					n = (short)va_arg(arg, int);
				else
					n = va_arg(arg, long);

				str = numstr(str, n, 16, width, prec, flags);

				break;

			case 'c':
				/* pad to right using spaces */
				if (!(flags & FL_LEFT_JUSTIFY))
					while (--width > 0)
						*str++ = ' ';

				*str++ = (char)va_arg(arg, int);

				while (--width > 0)
					*str++ = ' '; /* pad to left using spaces */

				break;

			case 's':
				s = va_arg(arg, char*);
				if (s == NULL)
					break;
				slen = strlen(s);

				if (prec < 0)
					prec = slen;
				else if (slen > prec)
					slen = prec;

				if (!(flags & FL_LEFT_JUSTIFY))
					while (slen < width--)
						*str++ = ' ';

				/* Add string to the output str */
				int i;
				for (i = 0; i < slen; i++)
					*str++ = *s++;
				while (slen < width--)
					*str++ = ' ';

				break;

			case 'p':
				flags |= FL_SMALL;
				flags |= FL_SPECIAL; /* Precede hex value with 0x */

				if (width == -1)
				{
					flags |= FL_ZEROPAD;
					width = 2 * sizeof(void*);
				}

				n = (long)va_arg(arg, void*);
				str = numstr(str, n, 16, width, prec, flags);
				break;

			case 'n':
				printed = va_arg(arg, int*);
				*printed = (str - buf);
				break;
			}

			fmt++;
			state++;
			break;

		case 6: /* Reset state machine, advance */
			state = 0;
			flags = 0;
			width = -1;
			prec = -1;
			len = 0;
			n = 0;
			break;
		}
	}

	/* Add null termination */
	*str = '\0';
	return str-buf;
}
