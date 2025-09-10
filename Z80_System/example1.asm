org $0000
    jp  start

DS  0040h - 3

org $0040
start:
	ld 		sp, 0200h
	ld 		b, 10
	call 	f_fibo
	ld		hl, msgstr
	call 	f_print
	ld 		hl, (var_b)
	call 	f_print_num
	halt

f_fibo:
	ld 		hl, 1
	ld 		(var_a), hl
	ld 		(var_b), hl
func_fibo_loop:
	ld 		HL, (var_a)
	ld 		d, h
	ld 		e, l
	ld 		HL, (var_b)
	ld 		(var_a), hl
	add 	hl, de
	ld 		(var_b), hl
	djnz	func_fibo_loop
	ret
var_a:
	db 	0,0
var_b:
	db 	0,0

f_print_num:
	ld 	b, 4
	call 	rotleft_ahl
	add 	$30
	out 	(2), a
	ld 	b, 4
	call 	rotleft_ahl
	add 	$30
	out 	(2), a
	ld 	b, 4
	call 	rotleft_ahl
	add 	$30
	out 	(2), a
	ld 	b, 4
	call 	rotleft_ahl
	add 	$30
	out 	(2), a
	ret

rotleft_ahl:
	xor a
rotleft_loop:
	rl 	l
	rl 	h
	rla
	djnz rotleft_loop
	ret

f_print:
print_loop:
	ld	a, (HL)
	or	a
	ret	z
    ld  bc, $0002
	out	(c),a
	inc HL
	jp	print_loop

msgstr:
	db	13,10
	db	"Good evening everyone!!"
	db  13, 10
	db	0
