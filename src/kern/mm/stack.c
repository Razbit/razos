/* This file is a part of the RazOS project
 *
 * stack.h -- stack-related things
 *
 * Razbit 2015 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#include "kmalloc.h"
#include "../kio.h"
#include "stack.h"

struct stack_t* kstack = NULL;

void init_stack(uint32_t esp)
{
    kstack = kmalloc(sizeof(struct stack_t));
    kstack->start = (void*)esp;
    kstack->size = 0xFFFF; 
}

void dump_stack(struct stack_t* stack)
{
    unsigned int i, j = 0;
    uint32_t esp;
    __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
    kprintf("stack: 0x%p -> 0x%p\n", stack->start, esp);
    do
    {
        kprintf("%8.8x  ", (uint32_t)stack->start-j);
  
        for (i = 0; i < 16; i++)
        {
            if (i > 0 && i % 8 == 0)
                kprintf(" ");
            
            kprintf("%2.2x ", *((uint8_t*)stack->start-j-i));
        }

        if (i < 8)
            kprintf(" ");
        
        for (; i < 16; i++)
            kprintf("   ");
        kprintf(" |");
        
        for (i = 0; i < 16; i++)
        {
            if (isprint(*((uint8_t*)stack->start-j-i)))
                kprintf("%c", *((uint8_t*)stack->start-j-i));
            else
                kprintf(".");
        }
        
        kprintf("|\n");
        j += 16;
        
    } while (j < ((uint32_t)stack->start - esp));
}
