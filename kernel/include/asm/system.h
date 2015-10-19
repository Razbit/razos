/* This file is a part of the RazOS project
 *
 * system.h -- some global kernel-mode things
 *
 * Razbit 2015
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)

#endif /* SYSTEM_H */
