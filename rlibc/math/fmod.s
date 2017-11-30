/* This file is a part of the RazOS project
 *
 * Itara20 2016
 */

.global fmod

.section .text

fmod:
    fld 12(%esp)  /* y*/
    fld 4(%esp)   /* x*/

loop:
    fprem
    fnstsw %ax
    sahf
    jp loop

end:
    ret
