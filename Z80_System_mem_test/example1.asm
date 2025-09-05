org $0000
    jp  start

DS  0040h - 3

org $0040
start:
	ld	sp, 0100h
	ld	hl, msgstr
	call	func_print

	halt

func_print:
print_loop:
	ld	a,(hl)
	or	a
	ret	z
    ld  bc, $0002
	out	(c),a
	inc	hl
	jp	print_loop

msgstr:
	db	13,10
	db	"Good evening everyone!!"
	db  13, 10
	db	0

	end
