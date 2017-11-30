/* idt.h -- Interrupt Descriptor Table */

/* Copyright (c) 2015 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#ifndef IDT_H
#define IDT_H

void idt_init();

void except_err(uint32_t num, uint32_t err, uint32_t eip);
void except(uint32_t num, uint32_t eip);

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13(); /* GPF */
void isr14(); /* PF */
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr30();
void isr32(); /* PIT */
void isr33(); /* KB */
void isr39(); /* LPT1 or spurious */

#endif /* IDT_H */
