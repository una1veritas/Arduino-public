
	ld		sp, 0100h
	ld		hl, msg
	call	printmsg

	halt

printmsg:
	ld	a,(hl)
	or	a
	ret	z
	out	(2),a
	inc	hl
	jp	printmsg

msg:
	db	13,10
	db	"Good morning, ladies and gentlemen!"
	db  13, 10
	db	"This is Z80 cpu, 8-bit computer!"
	db  13, 10
	db	0

	end
