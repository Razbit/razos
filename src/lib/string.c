/* A partial implementation of the C-string library
 *
 * Razbit 2014 */

#include "string.h"
#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

void* memset(void* ptr, char value, int num)
{
	num--;
	char* str = ptr;
	
	for (;num >= 0; num--)
		str[num] = value;

	return ptr;
}

void* memcpy(void* dest, const void* source, size_t num)
{
	uint8_t* dst = dest;
	const uint8_t* src = source;

	if (num == 0)
		return dest;
	
	while (num)
	{
		*dst++ = *src++;
		num--;
	}
	
	return dest;
}

void* memmove(void* dest, const void* source, size_t num)
{
	if (num == 0)
		return dest;

	/* when source is higher than dest, we can just use memcpy() */
	if (source > dest)
		return memcpy(dest, source, num);

	else
	{
		uint8_t* dst = dest;
		const uint8_t* src = source;

		num--;
		
		dst += num;
		src += num;
		
		while (num)
		{
			*dst-- = *src--;
			num--;
		}
	
		return dest;
	}
}

char* strcpy(char* dest, const char* source)
{
	while (*source != '\0')
	{
		*dest++ = *source++;
	}

	*dest = *source; /* terminating null character */

	return dest;
}

char* strncpy(char* dest, const char* source, size_t num)
{
	int i = 0;
	bool terminated = false;
	
	for (;i < num; i++)
	{
		if (*source == '\0')
			terminated = true;

		if (terminated)
			*dest++ = '\0';
		else
			*dest++ = *source++;
	}

	return dest;
}

char* strcat(char* dest, const char* source)
{
	char* start = dest; 

	while (*start != '\0')
		start++;

	/* start now points to  the null character terminating the string 'dest'*/

	while (*source != '\0')
		*start++ = *source++;

	start = '\0';

	return dest;	
}

char* strncat(char* dest, const char* source, size_t num)
{
	char* start = dest; 

	while (*start != '\0')
		start++;

	/* start now points to  the null character terminating the string 'dest'*/

	int i = 0;

	while ((*source != '\0') && (i < num))
	{
		*start++ = *source++;
		i++;
	}

	start = '\0';

	return dest;
}
