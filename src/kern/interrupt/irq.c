/* This file is a part of the RazOS project
 *
 * irq.c -- Interrupt Request system
 *
 * Razbit 2014 */

#include "irq.h"
#include "isr.h"
#include "idt.h"

#include "../kio.h"

#include <portio.h>
#include <stddef.h>

/* Contains function pointers to the 16 IRQ handler functions */
irq_handler_t* handlers[] =
{
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL
};

static void irq_remap();

void init_irq()
{
    kprintf("Initializing IRQ handlers\n");
    irq_remap();

    idt_set(32, (uint32_t)irq0, 0x08, 0x8E);
    idt_set(33, (uint32_t)irq1, 0x08, 0x8E);
    idt_set(34, (uint32_t)irq2, 0x08, 0x8E);
    idt_set(35, (uint32_t)irq3, 0x08, 0x8E);
    idt_set(36, (uint32_t)irq4, 0x08, 0x8E);
    idt_set(37, (uint32_t)irq5, 0x08, 0x8E);
    idt_set(38, (uint32_t)irq6, 0x08, 0x8E);
    idt_set(39, (uint32_t)irq7, 0x08, 0x8E);
    idt_set(40, (uint32_t)irq8, 0x08, 0x8E);
    idt_set(41, (uint32_t)irq9, 0x08, 0x8E);
    idt_set(42, (uint32_t)irq10, 0x08, 0x8E);
    idt_set(43, (uint32_t)irq11, 0x08, 0x8E);
    idt_set(44, (uint32_t)irq12, 0x08, 0x8E);
    idt_set(45, (uint32_t)irq13, 0x08, 0x8E);
    idt_set(46, (uint32_t)irq14, 0x08, 0x8E);
    idt_set(47, (uint32_t)irq15, 0x08, 0x8E);

}

/* Called from assembly handler stub */
void irq_handler(struct register_t regs)
{
    /* Send end of interrupt (EOI) to the PICs
     *
     * If the interrupt came from the slave PIC */    
    if (regs.int_no >= 40)
        outb(0xA0, 0x20);
    /* Reset master PIC, too */
    outb(0x20, 0x20);

    /* If we have installed a handler for the recv'd IRQ,
     * we run that handler. Otherwise we default to printing
     * the IRQ and INT numbers (for now) */
    irq_handler_t handler = handlers[regs.int_no - 32];
    if (handler != NULL)
        handler(&regs);
    else
        kprintf("Received IRQ %#X that is bound to INT %#X\n", \
                regs.int_no - 32, regs.int_no);        
}

void install_irq_handler(int irq, irq_handler_t handler)
{
    handlers[irq-32] = handler;
}

void uninstall_irq_handler(int irq)
{
    handlers[irq] = NULL;
}

static void irq_remap()
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

