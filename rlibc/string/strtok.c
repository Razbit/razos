/* This file is a part of the RazOS project
 *
 * Razbit 2014 */

#include <string.h>

char* strtok(char* str, const char* delimiters)
{
	static char* last;
	if (str)
		last = NULL;

	int i = 0;
	int len = strlen(delimiters);

	if(len == 0)
		return NULL;

	/* if the original string has nothing left */
	if(!str && !last)
		return NULL;

	if(str && !last)
		last = str;

	/* start of the substr */
	char* p_start = last;

	while (1)
	{
		for (i = 0; i < len; i++)
		{
			if (*p_start == delimiters[i])
			{
				p_start++;
				break;
			}
		}

		if (i == len)
		{
			last = p_start;
			break;
		}
	}

	if (*last == '\0')
	{
		last = NULL;
		return NULL;
	}


	/* find the end of the substring, replace the delimiter with null */
	while(*last != '\0')
	{
		for(i = 0; i < len; i++)
		{
			if(*last == delimiters[i])
			{
				*last = '\0';
				break;
			}
		}

		last++;
		if (i < len)
			break;
	}

	return p_start;
}

