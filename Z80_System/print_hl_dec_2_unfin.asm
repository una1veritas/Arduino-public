
print_hl_dec2:
	ld 		ix, ten4pow
	ld 		c, 4
loop1:
	ld 		e, (ix) 	; ten ith power
	inc 	ix
	ld 		d, (ix)
	inc 	ix
	ld 		b, 0 		; the digit
loop0:
	sbc 	hl, de
	jp 		s, lesser
	inc 	b
	jr 		loop0
lesser:
	add 	hl, de 		; restore de
	ld 		a, b
	add 	$30
	out 	(2), a
	dec 	c
	jr 		nz, loop1
	ld 		a, l
	add 	$30
	out 	(2), a
	ret

ten4pow:
	dw		$2710
ten3pow:
	dw		$3e8
ten2pow:
	dw		$64
ten1pow:
	dw		$a
ten0pow:
	dw 		$1
