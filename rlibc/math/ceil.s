; This file is a part of the RazOS project
;
; Itara20 2016

[GLOBAL ceil]

SECTION .data

old:	dw 0
new:	dw 0x0800


SECTION .text

ceil:
	fstcw	[old]
	mov	eax, old
	and	ah, 0b11110011
	or	eax, new

	fldcw	[new]
	fld	QWORD [esp+4]
	frndint

	fldcw	[old]
	ret

