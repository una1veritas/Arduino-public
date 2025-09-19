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
		org 	0020h
addr:	dw		$0
valu:	dw 		$0
status:	db 		0, 0
lbuf:	ds 		16, 0

        org     0040h
mon:            ;entry point
		ld 		ix, addr
		ld 		iy, status
read_line:
		call	getln
		ld 		hl, lbuf
		; item 0
		ld 		(iy), 0
		ld 		a, (hl)
		cp 		$0
		jr 		z, default_dump
		;
		cp 		'H'
		jr 		z, mon_halt
		cp 		'.'
		jr 		nz, __skip1

		; dump with the end address after '.'
		inc 	hl
		ld 		c, 4
		call 	hexstr_de
		ld 		(ix+2), de
		ld 		hl, (ix)
loop_dump:
		call 	dump
		ld 		a, h
		cp 		(ix+2)
		jp 		s, ___skip
		jr 		z, ___low_8
		jr 		___skip
___low_8:
		ld 		a, l
		cp 		(ix+1)
		jp 		s, ___skip
		jr 		z, ___skip
		; loop dump
		jr 		loop_dump
___skip:
		jr 		read_line

__skip1:
		cp 		':'
		jr 		nz, __skip2
		ld 		(iy), 3 ; write bytes loop
		inc 	hl
		ld 		c, 2
		call 	hexstr_de
		ld 		(ix), e
		ld 		hl, addr
		ld 		(hl), e
		; loop dump
		jr 		read_line

__skip2:
		;
read_hexstr:
		ld 		c, 4
		call 	hexstr_de
		ld 		(ix), de
default_dump:
		ld 		hl, (ix)
		call 	dump
		ld 		(ix), hl
		; call 	print_endl
		jr 		read_line
; 
mon_halt:
		halt

; バッファ方式にしてるから最大4ニブルを一気に読んだ方がかんたんでは？


; subroutines

getln:
		ld 		hl, lbuf 	; buf ptr
		ld 		(hl), 0
		ld 		b, 0		; char count
		ld 		de, (ix)

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
;
hex2nib:
		cp      'a'     ; check whether a lower case
		jr      c, hex2nib_alnum  ; a digit or an upper case if carry set
		and     $df     ; lower case to upper case
hex2nib_alnum:
		cp      '9' + 1 ; check whether a digit
		jr      nc, hex2nib_alpha  ; possibly an upper case letter if carry not set
		sub     a, '0'  ; digit to int value
		jr      c, hex2nib_err     ; it was not '0' - '9' 
		ret
hex2nib_alpha:
		cp      'F'+1   ; check the digit whether less than 'F'
		jr      nc, hex2nib_err      ; error if it is larger than 'F'
		sub     'A'  	; A - F to integer 0 - 5
		jr      c, hex2nib_err       ; it was not 'A' - 'F'
		add     a, 10   ; A - F to 10 - 15 by +5
		ret

hex2nib_err:
		or 		$ff
		ret


; read hexadecimal string char upto 2 or 4 (set in c) 
; bytes from (hl) and return int val in de
;
hexstr_de:
    ld      de, 0000h
hexstr_de_lp:
    ld      a, (hl)
	call 	hex2nib
	cp 		$ff
	ret 	z
	and 	a		; clear Carry bit
	ld 		b, 4
hexstr_de_rl4:
    rl      e		 ;rotate left entire de
    rl      d
    djnz    hexstr_de_rl4
	add 	e
	ld 		e, a
    inc     hl
	dec 	c
    jr      nz, hexstr_de_lp
	ret


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

; dump memory b bytes from address in hl
dump:
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
	ld 		b, 16
dump_16:
	ld 		a, (hl)
	call 	print_byte
	ld 		a, ' '
	out 	(2), a
	inc 	hl
;
	djnz 	dump_16

dump_exit:
	ret
