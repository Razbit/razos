/* This file is a part of the RazOS project
 *
 * isr.c -- interrupt handling
 *
 * Razbit 2014 */

#include "isr.h"

#include "../kio.h"
#include <string.h>
#include <sprintf.h>

const char* exceptions[] =
{
    "Divide-by-Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Unavailable",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "", /* Reserved */
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "", "", "", "", "", "", "", "", "",  /* 21..29 Reserved */
    "Security Exception"
};

static void make_errmsg(char* str, struct register_t* regs);

/* Called from asm */
void isr_handler(struct register_t regs)
{
    char mesg[100];
    memset (mesg, 0, 100);

    make_errmsg(mesg, &regs);
    
    kprintf("%s\n", mesg);
}

static void make_errmsg(char* str, struct register_t* regs)
{
    char begin[10];
    memset(begin, 0, 10);

    if (regs->int_no == 2 || regs->int_no == 15       \
        || (regs->int_no >= 21 && regs->int_no <= 29) \
        || regs->int_no == 31)
    {
        strncpy(begin, "INTERRUPT", 10);
        if (regs->int_no == 2)
            sprintf(str, "[[%s %#X, %s]]", begin, regs->int_no, \
                    exceptions[regs->int_no]);
        else
            sprintf(str, "[[%s %#X]]", begin, regs->int_no);
    }
    else if (regs->int_no == 8 || regs->int_no == 18)
    {
        strncpy(begin, "ABORT", 10);
        if (regs->int_no == 8)
        {
            sprintf(str, "[[%s %#X, %s: %#X]]", begin, regs->int_no,   \
                    exceptions[regs->int_no], regs->err_code);
        }
        else
        {
            sprintf(str, "[[%s %#X, %s]]", begin, regs->int_no,   \
                    exceptions[regs->int_no]);
        }
    }        
    else if (regs->int_no == 3 || regs->int_no == 4)
    {
        strncpy(begin, "TRAP", 10);
        sprintf(str, "[[%s %#X, %s]]", begin, regs->int_no, \
                exceptions[regs->int_no]);
    }
    else
    {
        strncpy(begin, "FAULT", 10);
        if (regs->int_no < 10 || regs->int_no == 16 || regs->int_no == 19 \
            || regs->int_no == 20)
        {
            sprintf(str, "[[%s %#X, %s]]", begin, regs->int_no, \
                    exceptions[regs->int_no]);
        }
        else
        {
            sprintf(str, "[[%s %#X, %s: %#X]]", begin, regs->int_no, \
                    exceptions[regs->int_no], regs->err_code);
        }
    }
}
