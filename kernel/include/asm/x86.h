/* This file is a part of the RazOS project
 *
 * x86.h -- x86 architecture-specific thingies
 *
 * Razbit 2016 */

#ifndef X86_H
#define X86_H

/* Control registers */
/* CR0: operating mode, CPU states */
#define CR0_PG (1 << 31) /* Paging */
#define CR0_CD (1 << 30) /* Disable caching */
#define CR0_NW (1 << 29) /* When clear, enable write-through */
#define CR0_AM (1 << 18) /* Alignment check enabled */
#define CR0_WP (1 << 16) /* Supervisor can't write to read-only pages */
#define CR0_NE (1 << 5)  /* x87 FPU error reporting mode */
#define CR0_ET (1 << 4)  /* 387dx support (386, 486) */
#define CR0_TS (1 << 3)  /* x87/mmx/sse context saving delayed */
#define CR0_EM (1 << 2)  /* If set, no FPU available */
#define CR0_MP (1 << 1)  /* Controls interaction of WAIT with CR0_TS */
#define CR0_PE (1 << 0)  /* Enable protected mode */

/* CR3: 31..12: page directory base */
#define CR3_PCD (1 << 4) /* Page-directory cache-disable bit */
#define CR3_PWT (1 << 3) /* Page-directory write-through bit */

/* CR4: architectural extensions, indicate operating system
 * support for specific processor capabilities */
#define CR4_OSFXSR (1 << 9) /* Support for FXSAVE, FXRSTOR */
#define CR4_PGE (1 << 7) /* Enable global pages */
#define CR4_MCE (1 << 6) /* Enable machine-check exception */
#define CR4_PAE (1 << 5) /* Enables PAE */
#define CR4_PSE (1 << 4) /* Enables 4 MiB pages when set */
#define CR4_DE (1 << 3)  /* Debugging extensions */
#define CR4_TSD (1 << 2) /* Restrict RDTSC to PL 0 */
#define CR4_PVI (1 << 1) /* Protected-Mode Virtual Interrupts */
#define CR4_VME (1 << 0) /* Enable interrupt/exception handling in
                          * virtual-8086 mode */

#endif /* X86_H */
