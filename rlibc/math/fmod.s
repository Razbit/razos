[GLOBAL fmod]

SECTION .text

fmod:
	fld	QWORD [esp+12]	; y
	fld	QWORD [esp+4]	; x

loop:
	fprem
	fnstsw	ax
	sahf
	jp	loop

end:
	ret
