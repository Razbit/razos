/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

char* strstr(const char* str1, const char* str2)
{
	unsigned int i;

	while (*str1 != '\0')
	{
		str1 = strchr(str1, *str2);
		if (!str1)
			return NULL;

		for (i = 1; i <= strlen(str2); i++)
		{
			if (str1[i] != str2[i])
				break;


		}

		if (i == strlen(str2))
			return (char*)str1;
		else
			str1++;
	}

	return NULL;
}
