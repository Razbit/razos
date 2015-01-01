/* This file is a part of the RazOS project
 *
 * hexdump.c -- UNIX hexdump -like utility
 *
 * Razbit 2014 */

/* This version only prints the kind of output you get when
 * passing -C to the UNIX hexdump */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int read(FILE* file, int n, uint8_t* buf)
{
    uint8_t* tmp = buf;
    int c = 0, i = 0;

    do
    {
        c = fgetc(file);
        if (c != EOF)
        {
            *(tmp++) = c;
            i++;
        }            
    } while (c != EOF && i < n);
        
    return i;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: hexdump <file>\n");
        return -1;
    }
    
    FILE* file = fopen(argv[1], "r");
    uint8_t* buf = malloc(16);
    
    if (buf == NULL)
    {
        fprintf(stderr, "Memory error\n");
        return -1;
    }

    int n, i, j = 0;

    do
    {
        printf("%8.8x  ", j);
        n = read(file, 16, buf);
  
        for (i = 0; i < n; i++)
        {
            if (i > 0 && i % 8 == 0)
                printf(" ");
            
            printf("%2.2x ", *(buf+i));
        }

        if (n > 0)
        {
            if (i < 8)
                printf(" ");
            
            for (; i < 16; i++)
                printf("   ");
            printf(" |");
        }
        
        for (i = 0; i < n; i++)
        {
            if (isprint(*(buf+i)))
                printf("%c", *(buf+i));
            else
                printf(".");
        }
        
        if (n > 0)
            printf("|\n");
        
        j += n;
    } while (n > 0);

    printf("\n");
    
    return 0;
}
