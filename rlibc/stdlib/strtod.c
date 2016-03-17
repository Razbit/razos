/* This file is a part of the RazOS project
 *
 * Itara20 2016 */

#include <stdlib.h>
#include <ctype.h> /* isspace, isdigit, tolower, isalnum */
#include <math.h>  /* pow */
#include <errno.h>

static int strcicmp(char const *a, char const *b)
{
	for (;; a++, b++)
	{
		int d = tolower(*a) - tolower(*b);
		if (*a == '\0')  return 0;
		else if (d != 0) return d;
    }
}

static double digits_to_num(const char *str)
{
	double result = 0, e = 1;
	while (isdigit(*str)) result += (*str-- - '0') * e, e *= 10;
	return result;
}

static double parse_number(const char *str, char **endptr)
{
	while (isdigit(*str)) ++str; /* Pointer to the end of the parsed part. */
	*endptr = (char*)str--;
	return digits_to_num(str);
}

static double parse_e_format(const char *str, char **endptr)
{
	if (*str == 'e' || *str == 'E')
	{
		if (isdigit(*(str+1)))    return pow(10, parse_number(str+1, endptr));
		else if (*(str+1) == '+') return pow(10, parse_number(str+2, endptr));
		else if (*(str+1) == '-') return pow(0.1, parse_number(str+2, endptr));
	}
	return 1;
}

static double parse_decimal(const char *str, char **endptr)
{
	double result = 0, d = 1;

	result += parse_number(str, endptr);
	if (**endptr == '.')
	{
		++(*endptr);
		while (isdigit(**endptr)) ++(*endptr), d *= 10;
		result += digits_to_num(*endptr - 1) / d;
	}
	return result * parse_e_format(*endptr, endptr);
}

static double hex_digits_to_num(const char *str)
{
	double result = 0, e = 1;
	while (isxdigit(*str))
	{
		result += ((*str > '9') ? (*str &~ 0x20) - 'A' + 10 : (*str - '0')) * e;
		--str;
		e *= 16;
	}
	return result;
}

static double parse_hex_number(const char *str, char **endptr)
{
	while (isxdigit(*str)) ++str;
	*endptr = (char*)str--;
	return hex_digits_to_num(str);
}

static double parse_p_format(const char *str, char **endptr)
{
	if (*str == 'p' || *str == 'P')
	{
		if (isdigit(*(str+1)))    return pow(2, parse_number(str+1, endptr));
		else if (*(str+1) == '+') return pow(2, parse_number(str+2, endptr));
		else if (*(str+1) == '-') return pow(0.5, parse_number(str+2, endptr));
	}
	return 1;
}

static double parse_hex_decimal(const char *str, char **endptr)
{
	double result = 0, d = 1;

	result += parse_hex_number(str, endptr);
	if (**endptr == '.')
	{
		++(*endptr);
		while (isxdigit(**endptr)) ++(*endptr), d *= 16;
		result += hex_digits_to_num(*endptr - 1) / d;
	}
	return result * parse_p_format(*endptr, endptr);
}

/* Parse nan and infinity values. If parsed input does not represent nan
*  or infinity, return 0. */
static double check_nan_inf(const char *str, char **endptr)
{
	if (strcicmp("NAN", str) == 0)
	{
		*endptr = (char*)str+3;
		if (**endptr == '(') /* Handle NAN(sequences).*/
		{
			++(*endptr);
			while (isalnum(**endptr) || **endptr == '_') ++(*endptr);

			if (**endptr == ')') ++(*endptr);
			else				 *endptr = (char*)str+3;
		}
		return NAN;
	}
	else if (strcicmp("INF", str) == 0)
	{
		str += 3;
		if (strcicmp("INITY", str) == 0)
			str += 5;
		*endptr = (char*)str;
		return INFINITY;
	}
	return 0;
}

double strtod(const char *str, char **endptr)
{
	char **curend = (char**)(&str); /* In case endptr == NULL. */
	const char *orig = str;
	double sign = 1;

	while (isspace(*str)) ++str; /* Skip whitespace. */

	if		(*str == '+') ++str;
	else if (*str == '-') ++str, sign = -1;

	double result = check_nan_inf(str, curend);

	if (result != 0) return result * sign;

	if (strcicmp("0X", str) == 0 && isxdigit(*(str+2)))
		result = parse_hex_decimal(str+2, curend);
	else
		result = parse_decimal(str, curend);

	/* If no valid conversion, do not skip whitespace. */
	if (*curend == orig) *curend = (char*)orig;

	if (endptr != NULL) *endptr = *curend;

	if (isinf(result)) errno = ERANGE, result = HUGE_VAL;

	return result * sign;
}
