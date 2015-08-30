/* This file is a part of the RazOS project
 *
 * isr.h -- interrupt handling
 *
 * Razbit 2014 */

#ifndef ISR_H
#define ISR_H

#include <sys/types.h>

/* Holds CPU registers */
struct register_t
{
    uint32_t ds; /* DS selector */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* By pusha */
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss; /* CPU pushes automatically */
};

typedef void (*isr_handler_t)(struct register_t*);

void install_isr_handler(int isr, isr_handler_t handler);
void uninstall_isr_handler(int isr);

#endif /* ISR_H */

