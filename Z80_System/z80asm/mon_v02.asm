;
	    org 	0000h
rst:
	    ld 		sp, 0200h
	    jp  	mon
;
;;
; a  ... workspace reg.
; b  ... workspace reg. dnjz counter
; c  ... command
; de ... address
; ix ...  mon_curr_addr, curr_addr + 2 == end_addr
;


		org 	0010h
addr:	dw		$0
addr2:	dw 		$0
status:	db 		0, 0
lbuf:	ds 		$10, 0

        org     0030h
mon:            ;entry point
read_line:
		call	getln
		ld 		hl, lbuf
		ld 		a, (hl)
		cp 		$0
		jr 		z, default_dump
		;
		cp 		'H'
		jr 		z, mon_halt
		;
		cp 		'.'
		jr 		z, specify_end
		cp 		':'
		jr 		z, error
		;
		ld 		c, 4
		call 	hexstr_de
		ld 		(addr), de
		;ld 		a, d
		;call 	print_byte
		;ld 		a, e
		;call 	print_byte
		;
		cp 		$0
		jr 		z, default_dump
		cp		'.'
		jr 		z, specify_end
		jr 		error
		;
specify_end:
		inc 	hl 		; next to '.'
		ld 		c, 4
		call 	hexstr_de
		ld 		(addr2), de
		cp 		0
		jr 		nz, error
		;
default_dump:
		ld 		hl, (addr)
		ld 		de, (addr2)
		ld 		a, H
		cp 		d
		jr 		nz, cp_hl_de_end
		ld 		a, l
		cp 		e
cp_hl_de_end:
		jr 		z, cp_equal
		jr 		c, cp_less
		jr 		nc, cp_greater
cp_equal:	ld 	a, '='
		out 	(2), A
		jr 		do_dump
cp_less:	ld 	a, '<'
		out 	(2), A
		jr 		do_dump
cp_greater:	ld 	a, '>'
		out 	(2), A
		ld 		hl, (addr)
		ld 		de, $10
		add 	hl, de
		ld 		(addr2), hl
		jr 		do_dump
do_dump:
		call 	dump
		jr 		read_line
; 
error:
		ld 		hl, msg
		call 	print_str_hl
		ld 		hl, lbuf
		call 	print_str_hl
		jr 		read_line

msg:	db $0a, $0d
		db "error? "
		db $0a, $0d, 0

mon_halt:
		halt


; バッファ方式にしてるから最大4ニブルを一気に読んだ方がかんたんでは？


; subroutines

getln:
		ld 		hl, lbuf 	; buf ptr
		ld 		(hl), 0
		ld 		b, 0		; char count
		call 	print_endl
		ld 		a, '*'
		out 	(2), a

getln_wait:
		in 		a, (0)
		and 	a
		jr 		z, getln_wait
;
; no echo back
		in 		a, (1)
		cp 		$08 	;backspace
		jr 		z, getln_bkspc
		cp 		$7f		; del
		jr 		z, getln_bkspc
		cp 		$0a
		jr 		z, getln_end
		cp 		$0d
		jr 		z, getln_end
; other ctrl codes
		cp 		$20
		jr 		nc, getln_echo_proceed
		jr 		getln_wait

getln_bkspc:
		ld 		a, b
		and 	a
		jr 		z, getln_wait
		ld 		a, $08
		out 	(2), a
		ld 		a, ' '
		out 	(2), a
		ld 		a, $08
		out 	(2), a
		dec 	hl
		ld 		(hl), $0
		dec 	b
		jr 		getln_wait

getln_echo_proceed:
		out 	(2), a 		; echo back
		;
		ld 		(hl),a		; *ptr++ = a
		inc 	hl
		ld 		(hl), $0	; *ptr = NULL
		inc 	b
		ld 		a, b
		cp 		15
		jr 		nc, getln_end  ; force terminate line
		jr 		getln_wait

getln_end:	; parse lbuf
		ret


; convert one char expressing a hexadecimal digit 
; in A reg. to nibble in A
; bit 7 is set to A if A is not hex-dec char
;
hex2nib:
		cp 		'0'
		jr 		c, hex2nib.err
		cp 		'9' + 1
		jr 		nc, hex2nib.toupper
		sub 	'0'
		ret
		;
hex2nib.toupper:
		cp 		'a'
		jr	 	c, hex2nib.alpha
		cp 		'f' + 1
		jr	 	nc, hex2nib.alpha
		and 	$df
hex2nib.alpha:
		cp     'A' 
		jr      c, hex2nib.err  
		cp      'F' + 1 
		jr      nc, hex2nib.err      ; error if it is larger than 'F'
		sub 	'A' - 10
		ret
		;
hex2nib.err:
		ld 		a, $ff 	; error code
		ret


; read hexadecimal string char upto 2 or 4 (set in C) 
; bytes from (HL) and return int val in DE
; if non hexdec char is encountered at (HL), returns with current de value without inc hl.
; if C upper-limit bytes has been read, returns with current de value with increment hl.
; A reg. hold the last char read from (HL).
;
hexstr_de:
    ld      de, 0000h
hexstr_de_lp:
    ld      a, (hl)
	ld 		b, a
	call 	hex2nib
	cp 		$ff
	jr 		nz, hexstr_de.hex2nib_succ
	ld 		a, b 	; recover original value of A
	ret 	 		; encountered non-hexdec char.
hexstr_de.hex2nib_succ:
	and 	a		; clear Carry bit
	ld 		b, 4
hexstr_de_rl4:
    rl      e		 ;rotate left entire de
    rl      d
    djnz    hexstr_de_rl4
	add 	e
	ld 		e, a
	inc 	hl 		; 
	dec 	c
    jr      nz, hexstr_de_lp
	ret 			; return after c bytes read


;
;
print_str_hl:
		ld 		a,(hl)
		and 	A
		ret 	z
		out 	(2), a
		inc 	hl
		jr 		print_str_hl

; print a nibble in A
print_nibble:
		and 	$0f
		cp 		$a
		add 	'0'
		cp 		':'
		jr 		c, print_nibble_out
		add 	7
print_nibble_out:
		out 	(2), a
		ret

; print a byte in A
print_byte:
		push 	af
		rlca
		rlca
		rlca
		rlca
		call 	print_nibble
		pop 	af
		call 	print_nibble
		ret

print_endl:
		ld 		a, $0a
		out 	(2), A
		ld 		a, $0d
		out 	(2), A
		ret

; dump memory from addr to addr+2 (value)
;
dump:
	ld 		hl, (addr)
	ld 		de, (addr2)
dump_header:
	call 	print_endl
	ld 		a, h
	call 	print_byte
	ld 		a, l
	call 	print_byte
	ld 		a, ' '
	out 	(2), a
	ld 		a, ':'
	out 	(2), a
	ld 		a, ' '
	out 	(2), a
;
	ld 		b, 16 	; up to 16 bytes
dump_16:
	ld 		a, (hl)
	inc 	hl
	ld 		(addr), hl
	call 	print_byte
	ld 		a, ' '
	out 	(2), a
cp_de_hl:
	ld 		a, d
	cp 		h
	jr 		nz, cp_de_hl.comp_end
	ld 		a, e
	cp 		l
cp_de_hl.comp_end:
	jr 		z, dump_exit
	jr 		c, dump_exit
;
	djnz 	dump_16
	jr 		dump_header

dump_exit:
	ld 		de, 0
	ld 		(addr2), de
	ret
