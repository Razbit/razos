/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include <stdio.h>

/* TODO: because of the buffer associated with vsprintf(), we should actually
 *       use vdprintf() as the base for the printf() family.. */
/* TODO: printing floats */

/* The basis of printf-family of function is here, the vsprintf().
 * Following is what this implementation can (will be able to) do.
 *
 * Format specifier prototype:
 * %[flags][width][.prec][len]specifier
 *
 * flags: -     Left justify                                        [ok]
 *        0     Pad left with zeros                                 [ok]
 *        +     Always print a sign                                 [ok]
 *        (spc) Write a space if no sign is written                 [ok]
 *        #     Used w/ specifiers o, x or X the value is           [ok]
 *              preceded by 0, 0x or 0X respectively.
 *
 * width: (num) Min. number of chars to be printed, pad w/ blanks.  [ok]
 *        *     Width is specified as an int in the argument list.  [ok]
 *
 * .prec: (num) For integers (d, i, o, u, x, X): specifies the min. [ok]
 *              number of digits to be written, padded w/ leading
 *              zeros if needed. Not truncated. Precision of 0
 *              means that no character for 0 is printed.
 *              For s: max number of chars to be printed.           [ok]
 *              If the period is specified without an explicit
 *              value, 0 is assumed.
 *        *     Precision is specified as an int in the arg list.   [ok]
 *
 *              d i       u o x X   c    s     p     n
 * len:   (n/a) int16_t   uint16_t  char char* void* int16_t*       [ok]
 *        h     int16_t   uint16_t                   int16_t*       [??]
 *        l     int32_t   uint32_t                   int32_t*       [??]
 *
 * spec:  d, i       signed decimal                                 [ok]
 *        u          unsigned decimal                               [ok]
 *        o          octal                                          [ok]
 *        x, X       lower and higher case hexadecimal              [ok]
 *        f, F       lower and upper case floating point            [  ]
 *        e, E       lower and upper case scientific notation       [  ]
 *        g, G       use shorter of %e or %f, lower and upper case  [  ]
 *        a, A       lower and upper case hex floating-point        [  ]
 *        c          unsigned char                                  [ok]
 *        s          C-string                                       [ok]
 *        p          a pointer                                      [ok]
 *        n          the number of chars printed so far is stored   [ok]
 *                   to this location. Nothing printed.
 *        %          a % character (0x25) is printed.               [ok]
 */

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

			state++;

		case 5: /* Conversion specifier */
			switch (*fmt)
			{
			case 'd':
			case 'i':
				flags |= FL_SIGN;
			case 'u':
				if (len == 0)
					str = numstr(str, (uint32_t)va_arg(arg, int),
					             10, width, prec, flags);
				else
					str = numstr(str, (uint32_t)va_arg(arg, uint32_t),
					             10, width, prec, flags);
				break;

			case 'o':
				if (len == 0)
					str = numstr(str, (uint32_t)va_arg(arg, int),
					             8, width, prec, flags);
				else
					str = numstr(str, (uint32_t)va_arg(arg, uint32_t),
					             8, width, prec, flags);
				break;

			case 'x':
				flags |= FL_SMALL;
			case 'X':

				if (len == 0)
					str = numstr(str, (uint32_t)va_arg(arg, int),
					             16, width, prec, flags);
				else
					str = numstr(str, (uint32_t)va_arg(arg, uint32_t),
					             16, width, prec, flags);
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
				if (width == -1)
				{
					flags |= FL_ZEROPAD;
					width = 8;
				}
				str = numstr(str, (uint32_t)va_arg(arg, uint32_t), 16,	\
				             width, prec, flags);
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
			break;
		}
	}

	/* Add null termination */
	*str = '\0';
	return str-buf;
}
