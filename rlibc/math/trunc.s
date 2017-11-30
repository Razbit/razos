/* This file is a part of the RazOS project
 *
 * Itara20 2016
 */

.global trunc

.section .data

old:
    .int old
    movw $0, old
new:
    .int new
    movw $0x0C00, new

.section .text

trunc:
    fstcw (old)
    movl (old), %eax
    andb $0b11110011, %ah
    orl (new), %eax

    fldcw (new)
    fld 4(%esp)
    frndint

    fldcw (old)
    ret
