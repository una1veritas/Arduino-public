
	ld	sp, 0030h
	ld	hl, msgipl
	call	prmsg

	halt
prmsg:
	ld	a,(hl)
	or	a
	ret	z
	out	(2),a
	inc	hl
	jp	prmsg

msgipl:
	db	13,10
	db	"hello, world!"
	db  13, 10
	db	0

	end
