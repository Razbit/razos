/* This file is a part of the RazOS project
 *
 * stack.h -- stack-related things
 *
 * Razbit 2015 */

#include <sys/types.h>

struct stack_t
{
    void* start; /* page align */
    size_t size; /* 16-byte align */
};

void init_stack(uint32_t esp);
void dump_stack(struct stack_t* stack);

