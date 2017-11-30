/* This file is a part of the RazOS project
 *
 * Itara20 2016
 */

.global floor

.section .data

old:
    .int old
    movw $0, old
new:
    .int new
    movw $0x0400, new


.section .text

floor:
    fstcw (old)
    movl (old), %eax
    andb $0b11110011, %ah
    orl (new), %eax

    fldcw (new)
    fld 4(%esp)
    frndint

    fldcw (old)
    ret
