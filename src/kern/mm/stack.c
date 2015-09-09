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
    kstack->start = (void*)(esp - STACK_SIZE);
    kstack->size = STACK_SIZE;
    kstack->offset = 0; /* We'll set this in the scheduler */
}

struct stack_t* create_stack(size_t size)
{
    struct stack_t* stack = kmalloc(sizeof(struct stack_t));
    stack->size = size;
    stack->start = kmalloc(size);

    kprintf("Created a stack at 0x%p of size 0x%x (%u KiB)\n", stack, size, size/0x40);
        
    return stack;
}

void dump_stack(struct stack_t* stack)
{
    uint32_t esp;
    __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
    dump_stack_esp(stack, esp);
}

void dump_stack_esp(struct stack_t* stack, uint32_t esp)
{
    unsigned int i, j = 0;
    uint32_t start = stack->start;
    uint32_t size = stack->size;
    uint32_t end = start + size;
    uint8_t data[16] = {0}, prev[16] = {1};

    kprintf("stack: 0x%p -> 0x%p\n", stack->start, esp);
    do
    {
        for (i = 0; i < 16; i++)
        {
            data[i] = *((uint8_t*)esp+j-i);
        }
        
        if (memcmp(&data[0], &prev[0], 16))
        {
            kprintf("%8.8x  ", esp + j);
            
            for (i = 0; i < 16; i++)
            {
                if (i > 0 && i % 8 == 0)
                    kprintf(" ");
                
                kprintf("%2.2x ", data[i]);
            }
            
            if (i < 8)
                kprintf(" ");
            
            for (; i < 16; i++)
                kprintf("   ");
            kprintf(" |");
            
            for (i = 0; i < 16; i++)
            {
                if (isprint(data[i]))
                    kprintf("%c", data[i]);
                else
                    kprintf(".");
            }
        
            kprintf("|\n");
        }
        j += 16;
        memcpy(&prev[0], &data[0], 16);
    } while (j < ((uint32_t)end - esp));
}
