/* This file is a part of the RazOS project
 *
 * Razbit 2014, 2016 */

#include <string.h>

int strcmp(const char* str1, const char* str2)
{
	int diff = (int)strlen(str1) - (int)strlen(str2);
	if (diff != 0)
		return diff;
	
	while (1)
	{
		if (*str1 < *str2)
			return -1;
		if (*str1 > *str2)
			return 1;
		if (*str1 == '\0') /* Actually, if both are null */
			return 0;

		str1++;
		str2++;
	}
}

int strncmp(const char* str1, const char* str2, size_t num)
{
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	for (; num > 0; num--)
	{
		/* str1 is too short */
		if ((len1 < num) && (len1 < len2))
			return -1;
		/* str2 is too short */
		if ((len2 < num) && (len2 < len1))
			return 1;
		if (*str1 < *str2)
			return -1;
		if (*str1 > *str2)
			return 1;
		if (*str1 == '\0') /* Actually, if both are null */
			return 0;

		str1++;
		str2++;
		len1--;
		len2--;
	}

	return 0;
}

