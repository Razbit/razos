/* This file is a part of the RazOS project
 *
 * isr.c -- interrupt handling
 *
 * Razbit 2014 */

#include "isr.h"

#include "../kio.h"
#include <string.h>
#include <sprintf.h>

isr_handler_t isr_handlers[256];

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

/* Called from interrupt.s */
void isr_handler(struct register_t regs)
{
    /* If we have an ISR handler installed, use that */
    if (isr_handlers[regs.int_no] != 0)
    {
        isr_handler_t handler = isr_handlers[regs.int_no];
        handler(regs);
    }
    else
    {
        char mesg[100];
        memset(mesg, 0, 100);
        make_errmsg(mesg, &regs);    
        kprintf("%s\n", mesg);
    }
}

void install_isr_handler(int isr, isr_handler_t handler)
{
    isr_handlers[isr] = handler;
}

void uninstall_isr_handler(int isr)
{
    isr_handlers[isr] = NULL;
}

static void make_errmsg(char* str, struct register_t* regs)
{
    char begin[10];
    memset(begin, 0, 10);

    if (regs->int_no == 2 || regs->int_no == 15       \
        || (regs->int_no >= 21 && regs->int_no <= 29) \
        || regs->int_no == 31)
    {
        strncpy(begin, "UNHANDLED INTERRUPT", 10);
        if (regs->int_no == 2)
            sprintf(str, "[[%s 0x%X, %s]]", begin, regs->int_no, \
                    exceptions[regs->int_no]);
        else
            sprintf(str, "[[%s 0x%X]]", begin, regs->int_no);
    }
    else if (regs->int_no == 8 || regs->int_no == 18)
    {
        strncpy(begin, "ABORT", 10);
        if (regs->int_no == 8)
        {
            sprintf(str, "[[%s 0x%X, %s: 0x%X]]", begin, regs->int_no,   \
                    exceptions[regs->int_no], regs->err_code);
        }
        else
        {
            sprintf(str, "[[%s 0x%X, %s]]", begin, regs->int_no,   \
                    exceptions[regs->int_no]);
        }
    }        
    else if (regs->int_no == 3 || regs->int_no == 4)
    {
        strncpy(begin, "TRAP", 10);
        sprintf(str, "[[%s 0x%X, %s]]", begin, regs->int_no, \
                exceptions[regs->int_no]);
    }
    else
    {
        strncpy(begin, "FAULT", 10);
        if (regs->int_no < 10 || regs->int_no == 16 || regs->int_no == 19 \
            || regs->int_no == 20)
        {
            sprintf(str, "[[%s 0x%X, %s]]", begin, regs->int_no, \
                    exceptions[regs->int_no]);
        }
        else
        {
            sprintf(str, "[[%s 0x%X, %s: 0x%X]]", begin, regs->int_no, \
                    exceptions[regs->int_no], regs->err_code);
        }
    }
}
