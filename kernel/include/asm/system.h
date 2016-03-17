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

#ifndef SYSTEM_H
#define SYSTEM_H

#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)

/* Bochs magic breakpoint */
#define BOCHS_BREAK __asm__ __volatile__("xchgw %%bx, %%bx":::"memory")

#endif /* SYSTEM_H */
