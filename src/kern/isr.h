/* This file is a part of the RazOS project
 *
 * isr.h -- interrupt handling
 *
 * Razbit 2014 */

#ifndef ISR_H
#define ISR_H

#include "../lib/stdint.h"

struct register_t
{
    uint32_t ds; /* DS selector */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* By pusha */
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss; /* CPU pushes automatically */
};

#endif /* ISR_H */

