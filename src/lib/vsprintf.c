/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include "stdarg.h"
#include "string.h"
#include "ctype.h"
#include "stdint.h"

#include "vsprintf.h"

#define FL_SIGN 0x1
#define FL_LEFT_JUSTIFY 0x2
#define FL_PLUS 0x4
#define FL_SPACE 0x8
#define FL_SPECIAL 0x10
#define FL_ZEROPAD 0x20
#define FL_SMALL 0x40

#define SIGNED_DECIMAL 0x1
#define UNSIGNED_DECIMAL 0x2
#define OCTAL 0x4
#define HEXADECIMAL 0x8
#define UNSIGNED_CHAR 0x10
#define CSTRING 0x20
#define POINTER 0x40
#define WRITE 0x80

static int atoi_ptr(const char **str)
{
    int i = 0;

    while (isdigit(**str))
        i = i * 10 + *((*str)++) - '0';

    return i;
}

char* numstr(char* str, int32_t num, int base, int width, int prec, int flags)
{
    /* Convert number num of radix base */

    char c, sign, tmp[36];
    const char* digs = "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;

    if (flags & FL_SMALL)
        digs = "01234567890abcdefghijklmnopqrstuvwxyz";

    if (flags & FL_LEFT_JUSTIFY)
        flags &= ~FL_ZEROPAD;

    if (base<2 || base>36)
        return 0;

    if (flags & FL_ZEROPAD)
        c = '0';
    else
        c = ' ';

    if (flags & FL_SIGN && num < 0)
    {
        sign = '-';
        num = -num;
    }
    else
    {
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

    if (sign)
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
            num = num/base;
        }
            
    }

    if (i > prec)
        prec = i;
    width -= prec;

    if (!(flags & (FL_ZEROPAD + FL_LEFT_JUSTIFY)))
        while (width-- > 0)
            *str++ = ' ';

    if (sign)
        *str++ = sign;

    if (flags & FL_SPECIAL)
    {
        if (base == 8)
            *str++ = '0';
        else if (base == 16)
        {
            *str++ = '0';
            *str++ = digs[33]; /* x or X */
        }        
    }
    
    if (!(flags & FL_LEFT_JUSTIFY))
        while (width-- > 0)
            *str++ = c;

    while (i < prec--)
        *str++ = '0';
    while (i-- > 0)
        *str++ = tmp[i];
    while (width-- > 0)
        *str++ = ' ';
     
    return str;
}

int vsprintf (char* buf, const char* fmt, va_list arg)
{
    uint8_t flags = 0;
    int state = 0;
    int width = 0;
    int prec = -1;
    int len = 0;
    int spec = 0;
    int *printed;
    
    char *str = buf;
    char *s;

    int slen;
    
    while (*fmt != '\0')
    {
        switch (state)
        {
        case 0: /* wait for % -char */
            if(*fmt != '%')
            {
                *str++ = *fmt++;
                break;
            }
            state++;
            fmt++;
            
        case 1: /* % received; wait for flags */
            if (*fmt++ == '%')
            {
                *str++ = '%';
                state = 0;
                break;
            }

            if (*fmt++ == '-')
            {
                flags |= FL_LEFT_JUSTIFY;
                break;
            }

            if (*fmt++ == '0')
            {
                flags |= FL_ZEROPAD;
                break;
            }

            if (*fmt++ == '+')
            {
                flags |= FL_PLUS;
                flags |= FL_SIGN;
                break;
            }

            if (*fmt++ == ' ')
            {
                flags |= FL_SPACE;
                break;
            }

            if (*fmt++ == '#')
            {
                flags |= FL_SPECIAL;
                break;
            }

            /* Flag 'precedence' */
            if ((flags & FL_PLUS) && (flags & FL_SPACE))
                flags &= ~FL_SPACE; /* Remove the space flag */
            
            if ((flags & FL_LEFT_JUSTIFY) && (flags & FL_ZEROPAD))
                flags &= ~FL_ZEROPAD;

            state++;
            
        case 2: /* Field width */
            /* TODO: width from arg. list using an asterisk (*) */
            if (isdigit(*fmt))
                width = atoi_ptr(&fmt);

            state++;

        case 3: /* precision */
            /* TODO: precision from arg. list using an asterisk (*) */
            if (*fmt != '.')
            {
                state++;
                break;
            }

            fmt++;

            prec = atoi_ptr(&fmt);
            state++;

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
                if (len = 0)
                    str = numstr(str, (uint32_t)va_arg(arg, int),
                                 10, width, prec, flags);
                else
                    str = numstr(str, (uint32_t)va_arg(arg, uint32_t),
                                 10, width, prec, flags);
                break;

            case 'o':
                if (len = 0)
                    str = numstr(str, (uint32_t)va_arg(arg, int),
                                 8, width, prec, flags);
                else
                    str = numstr(str, (uint32_t)va_arg(arg, uint32_t),
                                 8, width, prec, flags);
                break;

            case 'x':
                flags |= FL_SMALL;
            case 'X':
                
                if (len = 0)
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

                *str++ = (uint8_t)va_arg(arg, int);

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

                int i;
                for (i = 0; i < slen; i++)
                    *str++ = *s++;
                while (slen < width--)
                    *str++ = ' ';
                
                break;

            case 'p':
                str = numstr(str, (uint32_t)va_arg(arg, void*), 16, width, prec, flags);
                break;

            case 'n':
                printed = va_arg(arg, int*);
                *printed = (str - buf);
                break;
            }
            
            fmt++;
            state++;
            break;
        case 6:
            state = 0;
            flags = 0;
            width = 0;
            prec = -1;
            len = 0;
            spec = 0;
            break;
        }
    }

    *str = '\0';
    return str-buf;
}