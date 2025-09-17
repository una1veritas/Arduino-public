;
	    org 	0000h
rst:
	    ld 		sp, 0800h
	    jp  	mon
;
;;
; a  ... workspace reg.
; b  ... workspace reg. dnjz counter
; c  ... command
; de ... address
; ix ...  mon_curr_addr, curr_addr + 2 == end_addr
;
		org 	0080h
addr:	dw		$0
valu:	dw 		$0	
lbuf:	ds 		32, 0

        org     00a0h
mon:            ;entry point

; やっぱり、一行よむバッファがないと、delete/backspace も
; 処理できないから、getline タイプに改める。
; そうしないと、del/backspace で de レジスタを逆転するだけじゃなく、
; . や : を跨いでもどさないといけないし

getln:
		ld 		ix, addr
getln_prompt:
		ld 		hl, lbuf 	; buf ptr
		ld 		(hl), 0
		ld 		c, 0 		; parse status
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
		in 		a, (1)
		cp 		$08 	;backspace
		jr 		z, getln_bkspc
		cp 		$7f		; del
		jr 		z, getln_bkspc
		jr 		non_del

getln_bkspc:
		ld 		a, b
		and 	a
		jr 		z, getln_wait
		ld 		a, $08
		out 	(2), a
		ld 		a, ' '
		out 	(2), A
		ld 		a, $08
		out 	(2), a
		dec 	hl
		ld 		(hl), $0
		dec 	b
		jr 		getln_wait

non_del:
; return codes
		cp 		$0d
		jr 		z, line_ended
		cp 		$0a
		jr 		z, line_ended
; escape seq
;		cp 		$1b 		; ESC
;		jr 		z, ESC_SEQ
; other ctrl codes
		cp 		$20
		jr 		nc, __echo
		ld 		c, a
		ld 		a, '$'
		out		(2), a
		ld 		a, c
		call 	print_byte
		jr 		getln_wait

__echo:
		out 	(2), a 		; echo back

		ld 		(hl),a
		inc 	hl
		ld 		(hl), $0
		inc 	b
		ld 		a, b
		cp 		31
		jr 		z, line_ended  ; force terminate line
		jr 		getln_wait


line_ended:	; parse lbuf
		ld 		hl, lbuf
lbuf_parse_loop:
		ld 		a, (hl)
		and 	a
		jr 		z, end_parse
		cp 		'.'
		jr 		z, next_item

; ここバッファ方式にしてるから最大4ニブルを一気に読んだ方がかんたんでは？

		call 	hex2nibble
		cp 		a, $ff
		jr 		z, next_item 	; seems got error by unexpected char
; shift DE 4 bit and add the nibble in A 
		ld 		b, 4
_rl_4:
		and 	a		; clr carry
		rl		e
		rl 		d
		djnz 	_rl_4
		add 	e
		ld 		e, a
		ld 		a, c
		cp 		'.'
		jr  	z, valu_update
		ld 		(ix), de
		jr 		addr_updated
valu_update:
		ld 		(ix+2), de
addr_updated:
		inc 	hl
		jr 		lbuf_parse_loop

next_item:
		ld 		c, '.'
		jr 		lbuf_parse_loop

end_parse:
;		call 	print_endl ; echo back does end the line
		ld 		hl, (ix)
		call 	dump
		ld 		(ix), hl
		jp 		getln_prompt

; subroutines

; convert one char expressing a hexadecimal digit 
; in A reg. to nibble in A
;
hex2nibble:
    cp      'a'     ; check whether a lower case
    jr      c, hex2nibble_digit_or_upper  ; a digit or an upper case if carry set
    and     $df     ; lower case to upper case
hex2nibble_digit_or_upper:
    cp      '9' + 1 ; check whether a digit
    jr      nc, hex2nibble_upper  ; possibly an upper case letter if carry not set
    sub     a, '0'  ; digit to int value
    jr      c, hex2nibble_err     ; it was not '0' - '9' 
    ret
hex2nibble_upper:
    cp      'F'+1   ; check the digit whether less than 'F'
    jr      nc, hex2nibble_err      ; error if it is larger than 'F'
    sub     'A'  	; A - F to integer 0 - 5
    jr      c, hex2nibble_err       ; it was not 'A' - 'F'
    add     a, 10   ; A - F to 10 - 15 by +5
hex2nibble_output:
    ret

hex2nibble_err:
	ld 		a, 0xff
	ret

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
		ld 	a, $0d
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
;	jr 		dump_header

dump_exit:
	ret
