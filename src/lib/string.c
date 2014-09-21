/* A partial implementation of the C-string library
 *
 * Razbit 2014 */

#include "string.h"
#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "../monitor.h"
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

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
	const char* cptr1 = ptr1;
	const char* cptr2 = ptr2;
	
	for (; num > 0; num--)
	{
		if (*cptr1 < *cptr2)
			return -1;
		else if (*cptr1 > *cptr2)
			return 1;
		
		cptr1++;
		cptr2++;
	}
	
	return 0;
}

int strcmp(const char* str1, const char* str2)
{
	while (true)
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
	for (; num > 0; num--)
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

	return 0;
}

void* memchr(const void* ptr, char value, size_t num)
{
	const char* cptr = ptr;
	
	for (; num > 0; num--)
	{
		if (*cptr == value)
			return cptr;
		cptr++;
	}

	return NULL;
}

char* strchr(const char* str, char value)
{
	while (*str != '\0')
	{
		if (*str == value)
			return str;
		str++;
	}

	return NULL;
}

char* strrchr(const char* str, char value)
{
	const char* end = str;

	while (*end != '\0')
		end++;

	while (end >= str)
	{
		if (*end == value)
			return end;
		end--;
	}

	return NULL;
		
}

char* strpbrk(const char* str1, const char* str2)
{
	const char* temp = str2;
	
	while (*str1 != '\0')
	{
		while (*temp != '\0')
		{
			if (*str1 == *temp)
				return str1;
			temp++;
		}
		
		temp = str2;
		str1++;
	}

	return NULL;
}

size_t strcspn(const char* str1, const char* str2)
{
	size_t ret = 0;
	const char* temp = str2;
	
	while (*str1 != '\0')
	{
		while (*temp != '\0')
		{
			if (*str1 == *temp)
				goto strcspn_end;
			temp++;
		}
		
		temp = str2;
		str1++;
		ret++;
	}

strcspn_end:
	return ret;
}

size_t strspn(const char* str1, const char* str2)
{
	size_t ret = 0;
	const char* temp = str2;
	
	while (*str1 != '\0')
	{
		while (*temp != '\0')
		{
			if (*temp == *str1)
				ret++;
			temp++;
		}
		
		temp = str2;
		str1++;
	}

	return ret;
}

char* strstr(const char* str1, const char* str2)
{
	int i;
	
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
			return str1;
		else
			str1++;
	}

	return NULL;
}

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

	while (true)
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

size_t strlen(const char* str)
{
	size_t ret = 0;
	while (*str++ != '\0')
		ret++;

	return ret;
}
