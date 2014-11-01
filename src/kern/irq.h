/* This file is a part of the RazOS project
 *
 * irq.h -- Interrupt Request system
 *
 * Razbit 2014 */

#ifndef IRQ_H
#define IRQ_H

#include "isr.h"

typedef void (*irq_handler_t)(struct register_t *regs);

void init_irq();
void install_irq_handler(int irq, irq_handler_t handler);
void uinstall_irq_handler(int irq);

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif /* IRQ_H */
