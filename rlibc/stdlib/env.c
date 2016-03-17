/* env.c -- environment variable thingies */

/* Copyright (c) 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <errno.h>
#include <string.h>

char** environ = NULL;

/* Get value of environment variable */
char* getenv(const char* name)
{
	if (name == NULL || name[0] == '\0')
		return NULL;
	if (environ == NULL)
		return NULL;

	size_t i = 0;
	while (environ[i] != NULL)
	{
		if (strncmp(name, environ[i], strlen(name)) == 0)
		{
			/* Found the environment variable */
			char* val = strchr(environ[i], '=');
			if (val == NULL)
				return NULL; /* *should* never happen, but.. */
			val++; /* Val now points to the beginning of the value of
			        * the NAME=VAL pair */
			return val;
		}
		else
			i++;
	}

	return NULL;
}

/* Add or change environment variable */
int setenv(const char* envname, const char* envval, int overwrite)
{
	if (envname == NULL || envname[0] == '\0')
	{
		errno = EINVAL;
		return -1;
	}
	if (strchr(envname, '=') != NULL)
	{
		errno = EINVAL;
		return -1;
	}

	size_t index = 0;
	if (environ == NULL)
	{
		environ = (char**)malloc(2*sizeof(char**));
		if (environ == NULL)
		{
			errno = ENOMEM;
			return -1;
		}
		environ[1] = NULL; /* environ is NULL-terminated */
	}
	else
	{
		/* Modify */
		int found = 0;
		if (overwrite != 0)
		{
			while (environ[index] != NULL)
			{
				if (strncmp(envname, environ[index], strlen(envname)) == 0)
				{
					if (environ[index][strlen(envname)] == '=')
					{
						found = 1;
						break;
					}
				}
				index++;
			}
		}

		if (found == 0)
		{
			index = 0;
			/* Get size of environ, look for empty pair */
			while (environ[index] != NULL)
			{
				if (environ[index][0] == '\0')
				{
					found = 1;
					break;
				}
				index++;
			}
		}

		/* Add new variable to a freed index (or modify an existing one
		 * by rewriting it) */
	    if (found == 1)
		{
			free(environ[index]);
		}
	    else /* Add a new variable to the end of the list */
		{
			char** new_env = \
				(char**)realloc(environ, sizeof(char**)*(index+1));
			if (new_env == NULL)
			{
				errno = ENOMEM;
				return -1;
			}
			environ = new_env;
			environ[index+1] = NULL;
		}
	}

	/* index is now the index where we can put the new env variable */
	environ[index] = (char*)malloc(strlen(envname)+strlen(envval)+1);
	if (environ[index] == NULL)
	{
		errno = ENOMEM;
		return -1;
	}
	/* Copy the contents of envname and envval */
	strcpy(environ[index], envname);
	strcat(environ[index], "=");
	strcat(environ[index], envval);

	return 0;
}

/* Remove an environment variable */
int unsetenv(const char* name)
{
    if (name == NULL || name[0] == '\0')
	{
		errno = EINVAL;
		return -1;
	}
	if (strchr(name, '=') != NULL)
	{
		errno = EINVAL;
		return -1;
	}

	size_t i = 0;
	while (environ[i] != NULL)
	{
		if (strcmp(name, environ[i]) == 0)
		{
			/* Found the environment variable. Free the string
			 * and mark as empty (alloc one char, set to 0) for
			 * using the index again in setenv */
			free(environ[i]);
			environ[i] = (char*)malloc(sizeof(char));
			environ[i][0] = '\0';
			return 0;
		}
		else
			i++;
	}
	return 0; /* No such variable found, return successfully */
}
