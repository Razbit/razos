/* This file is a part of the RazOS project
 *
 * stack.h -- stack-related things
 *
 * Razbit 2015 */

#ifndef STACK_H
#define STACK_H

#include <sys/types.h>

#define STACK_SIZE 0xFFFF /* Stack size is 64 KB */

struct stack_t
{
    void* start;
    size_t size; /* 16-byte align */
    uint32_t offset; /* From (start + size) to ESP */
};

void init_stack(uint32_t esp);
struct stack_t* create_stack(size_t size);

void dump_stack(struct stack_t* stack);
void dump_stack_esp(struct stack_t* stack, uint32_t esp);

#endif /* STACK_H */
