;
; macros
clrf:	macro
		and 	a
		endm
;
clra: 	macro
		xor 	a
		endm
;
; I/O port
CONST 	equ 	$00
CONIO 	equ 	$01
;CONOUT equ 	$02
CLKMODE	equ 	$80
;
	    org 	$F000
; subroutines

; getlin
; read up to c bytes into buffer pointed by hl, end with 0
; hl ... line buffer pointer
; c  .... buffer limit length
getln:
		ld 		(hl), 0
		ld 		b, 0		; char count
		call 	print_endl
		ld 		a, '*'
		out 	(CONIO), a
        ;
getln_wait:
		in 		a, (CONST)
		and 	a
		jr 		z, getln_wait
;
; no echo back
		in 		a, (CONIO)
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
		out 	(CONIO), a
		ld 		a, ' '
		out 	(CONIO), a
		ld 		a, $08
		out 	(CONIO), a
		dec 	hl
		ld 		(hl), $0
		dec 	b
		jr 		getln_wait

getln_echo_proceed:
		out 	(CONIO), a 		; echo back
		;
		ld 		(hl),a		; *ptr++ = a
		inc 	hl
		ld 		(hl), $0	; *ptr = NULL
		inc 	b
		ld 		a, b
		cp 		c
		jr 		nc, getln_end  ; force terminate line
		jr 		getln_wait

getln_end:	; parse lbuf
		ret


; convert one char expressing a hexadecimal digit 
; in A reg. to nibble in A
; set carry flag if got a wrong char
;
hex2nib:
		cp 		'0'
		ret 	c		; error, return with carry flag set
		cp 		'9' + 1
		jr 		nc, hex2nib.toupper
		sub 	'0' 	; never sets carry flag
		ret				; no error, retuen the value without carry flag
hex2nib.toupper:
		and 	$df
		sub     'A' 
		ret      c		; error, return with carry flag set
		cp      'F' + 1
		jr      nc, hex2nib.err      ; error if it is larger than 'F'
		add 	10		;  never sets carry flag
		ret				; no error, return the value without carry
hex2nib.err:
		sub 	$ff 	; 	always sets carry flag
		ret


; read hexadecimal string char upto 2 or 4 (set in C) 
; bytes from (HL) and return int val in DE
; if non hexdec char is encountered at (HL), returns with current de value without inc hl.
; if C upper-limit bytes has been read, returns with current de value with increment hl.
; A reg. hold the last char read from (HL).
;
hexstr_de:
		ld      de, 0000h
hexstr_de.loop:
		ld      a, (hl)
		call 	hex2nib
		jr 		nc, hexstr_de.hex2nib_succ
		ld 		a, (hl) 	; recover original value of A
		ret 	 			; encountered non-hexdec char.
hexstr_de.hex2nib_succ:
		clrf			; clear Carry bit
		ld 		b, 4
hexstr_de.rl4:
		rl      e		 ;rotate left entire de
		rl      d
		djnz    hexstr_de.rl4
		add 	e
		ld 		e, a
		inc 	hl 		; 
		dec 	c
		jr      nz, hexstr_de.loop
		ret 			; return after c bytes read


;
;
print_str_hl:
		ld 		a,(hl)
		and 	A
		ret 	z
		out 	(CONIO), a
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
		out 	(CONIO), a
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
		out 	(CONIO), A
		ld 		a, $0d
		out 	(CONIO), A
		ret

; dump : dump memory from addr to addr+2 (value)
; hl ... start address (will be trucated)
; de ... end address
;
; bc ... the original start address

dump:
	ld 		b, h
	ld 		c, l
	ld 		a, $F0
	and 	l
	ld 		l, a
dump.print_header:
	call 	print_endl
	ld 		a, b
	call 	print_byte
	ld 		a, c
	call 	print_byte
	ld 		a, ' '
	out 	(CONIO), a
	ld 		a, ':'
	out 	(CONIO), a
	ld 		a, ' '
	out 	(CONIO), a
    ;
dump.bytes:
	;cp 	bc, hl
	ld 		a, b
	cp 		h
	jr 		nz, $+4
	ld 		a, c
	cp 		l
	;
	jr 		c, dump.print_byte
	jr 		z, dump.print_byte
; print two-spaces
	ld 		a, ' '
	out 	(CONIO), a
	out 	(CONIO), a
	jr 		dump.print_spc
dump.print_byte
	ld 		a, (hl)
	call 	print_byte
dump.print_spc:
	ld 		a, ' '
	out 	(CONIO), a
	inc 	hl
	; cp 	de, hl
	ld 		a, d
	cp 		h
	jr 		nz, $+4; dump.cp_de_hl_end
	ld 		a, e
	cp 		l
	; cp 	de, hl end
	ret 	z ; de == hl, then exit dump
	ret 	c ; de < hl, then exit dump
	ld 		a, l 	; test whether the least 4 bits of address is zero
	and 	$0f
	jr 		nz, dump.bytes
	ld 		b, h 
	ld 		c, l
	jr 		dump.print_header  ; if so print address header
    ;
;dump_exit:
	;ret

;
; print_err_msg
;
print_err_msg:
		push 	hl
		push 	af
		call 	print_endl
		ld 		hl, err_msg
		call 	print_str_hl
		pop 	af
		call 	print_byte
		call 	print_endl
		pop 	hl
		ld 		a, h
		call 	print_byte
		ld 		a, l
		call 	print_byte
		ret
		;
err_msg:
		db 	$0a, $0d, "error", $0a, $0d, 0
