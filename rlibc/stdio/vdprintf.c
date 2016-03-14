/* This file is a part of the RazOS project
 *
 * vdprintf.c -- vdprintf, a POSIX-addition to C99
 *
 * Razbit 2014, 2016, Itara20 2016 */

#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>

#include <stdio.h>

/* TODO: printing floats */

/* The basis of printf-family of function is here, the vdprintf().
 * This function is used for all other functions of the printf-family,
 * for now. It will be replaced by vfprintf because of heavy write()
 * usage. (syscall at every write -> slow as fuck)
 * 
 * Following is what this implementation can do. Aiming for C99..
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
 * len:   (n/a) int       int       char char* void* int*           [ok]
 *        h     short     short                      short*         [ok]
 *        l     long      long                       long*          [ok]
 *
 * spec:  d, i       signed decimal                                 [ok]
 *        u          unsigned decimal                               [ok]
 *        o          octal                                          [ok]
 *        x, X       lower and higher case hexadecimal              [ok]
 *        f, F       lower and upper case floating point            [ok]
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

/* When we write using wbuf_write(), we write to this buffer.
 * After 16 bytes, it gets flushed automagically,
 * or by manually calling wbuf_flush() */

static size_t wbuf_size = 0;
static char wbuf[16];
static size_t nprinted = 0;

static int fd = 0;

static ssize_t wbuf_flush()
{
	if (wbuf_size > 0)
	{
		ssize_t ret = write(fd, wbuf, wbuf_size);
		wbuf_size = 0;
		return ret;
	}
	return 0;
}

static ssize_t wbuf_write(const char c)
{
	wbuf[wbuf_size++] = c;
	nprinted++;
	if (wbuf_size == 16)
		return wbuf_flush();
	return 0;
}

static int atoi_ptr(const char **str)
{
	int i = 0;

	while (isdigit(**str))
		i = i * 10 + *((*str)++) - '0';

	return i;
}

static void numstr(long number, int base, int width, int prec, int flags)
{
/* Convert number [num] of radix [base] */
/* Not much to explain, just fancy pointer fiddling */

	char c, sign, tmp[36];
	const char* digs = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;
	unsigned long num;

	if (width < 0)
		width = -width;

	if (flags & FL_SMALL)
		digs = "0123456789abcdefghijklmnopqrstuvwxyz";

	if (base < 2 || base > 36)
		return;

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
			wbuf_write(' ');

	if (sign)
		wbuf_write(sign);

	if (flags & FL_SPECIAL)
	{
		if (base == 8)
		{
			wbuf_write('0');
		}
		else if (base == 16)
		{
			wbuf_write('0');
			wbuf_write(digs[33]); /* x or X */
		}
	}

	if (!(flags & FL_LEFT_JUSTIFY))
		while(width-- > 0)
			wbuf_write(c);

	while (i < prec--)
		wbuf_write('0');

	while (i-- > 0)
		wbuf_write(tmp[i]);

	while (width-- > 0)
		wbuf_write(' ');
}

static void floatstr(double number, int width, int prec, int flags)
{
	const int16_t buf_size = 1024;
	char c, sign;
	char tmp [buf_size];
	const char* digs = "0123456789";
	int i, p;
	double num, frac;
	
	if (prec > buf_size)
		prec = buf_size;

	if (width < 0)
		width = -width;

	if (flags & FL_ZEROPAD)
		c = '0';
	else
		c = ' ';
	
	if (number < 0)
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
	
	if (isnan(number))
	{
		if (flags & FL_SMALL)
			tmp[2] = 'n', tmp[1] = 'a', tmp[0] = 'n';
		else
			tmp[2] = 'N', tmp[1] = 'A', tmp[0] = 'N';
		
		width -= 3;
		i = 3;
		prec = 0;
	}
	else if (isinf(number))
	{
		if (flags & FL_SMALL)
			tmp[2] = 'i', tmp[1] = 'n', tmp[0] = 'f';
		else
			tmp[2] = 'I', tmp[1] = 'N', tmp[0] = 'F';
		
		width -= 3;
		i = 3;
		prec = 0;
	}
	else
	{
		i = 0;
		frac = modf(num, &num);

		if (frac == 0)
		{
			if (!prec)
			{
				if (flags & FL_SPECIAL)
					tmp[i++] = '.';
			}
			else
			{
				i += prec;
				tmp[i--] = '.';
				for (p = prec; p > 0; p--)
					tmp[i--] = '0';
				i += prec + 2;
			}
		}
		else
		{
			i += prec;
			tmp[i--] = '.';
			for (p = prec; p > 0; p--)
			{
				frac *= 10;
				tmp[i--] = digs[(int)frac];
				frac = modf(frac,&frac);
			}
			i += prec + 2;
		}

		if (num == 0.0)
			tmp[i++] = '0';
		else
		{
			while (num >= 1)
			{
				tmp[i++] = digs[(int)fmod(num, 10.0)];
				num /= 10.0;
			}
		}
		
		if (i > prec)
			prec = i;
		
		width -= prec;
	}

	if (!(flags & (FL_LEFT_JUSTIFY+FL_ZEROPAD)))
		while (width-- > 0)
			wbuf_write(' ');

	if (sign)
		wbuf_write(sign);

	if (!(flags & FL_LEFT_JUSTIFY))
		while(width-- > 0)
			wbuf_write(c);

	while (i < prec--)
		wbuf_write('0');

	while (i-- > 0)
		wbuf_write(tmp[i]);

	while (width-- > 0)
		wbuf_write(' ');
}

int vdprintf(int fd_, const char* fmt, va_list arg)
{
	uint8_t flags = 0;
	int state = 0;
	int width = -1;
	int prec = -1;
	int len = 0;
	int* printed;
	nprinted = 0;
	long n;

	char* s;

	int slen;

	fd = fd_;

	/* State-machine: Go through every char in fmt, do things with it */
	while (*fmt != '\0')
	{
		switch (state)
		{
		case 0: /* wait for % -char. Until found, keep printing */
			if(*fmt != '%')
			{
				wbuf_write(*fmt++);
				break;
			}
			state++;
			fmt++;

		case 1: /* % received; wait for flags */
			if (*fmt == '%') /* If we got another %, print it */
			{
				wbuf_write(*fmt++);
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
				numstr(n, 10, width, prec, flags);
				break;

			case 'o':
				if (len == 0)
					n = va_arg(arg, int);
				else if (len == -1)
					n = (short)va_arg(arg, int);
				else
					n = va_arg(arg, long);
				numstr(n, 8, width, prec, flags);
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
				numstr(n, 16, width, prec, flags);
				break;

			case 'c':
				/* pad to right using spaces */
				if (!(flags & FL_LEFT_JUSTIFY))
					while (--width > 0)
						wbuf_write(' ');

				wbuf_write((char)va_arg(arg, int));

				while (--width > 0)
					wbuf_write(' '); /* pad to left using spaces */

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
						wbuf_write(' ');

				/* Add string to the output str */
				int i;
				for (i = 0; i < slen; i++)
					wbuf_write(*s++);
				while (slen < width--)
					wbuf_write(' ');

				break;

			case 'p':
				flags |= FL_SMALL;
				if (width == -1)
				{
					flags |= FL_ZEROPAD;
					width = 8;
				}
				numstr((long)va_arg(arg, int), 16,	\
				       width, prec, flags);
				break;

			case 'n':
				printed = va_arg(arg, int*);
				if (len == 0)
					*printed = (int)nprinted;
				else if (len == -1)
					*printed = (short)nprinted;
				else
					*printed = (long)nprinted;
				break;
			
			case 'f':
				flags |= FL_SMALL;
			case 'F':
				if (prec == -1) /* Default precision is 6. */
					prec = 6;
				floatstr(va_arg(arg, double), width, prec, flags);
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
	wbuf_write('\0');
	wbuf_flush();
	return nprinted;
}
