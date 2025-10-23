; macros
clrcf:	macro		; clear carry flag
		and 	a
		endm
;
clra: 	macro
		xor 	a
		endm
;
; I/O port
;
CONSTA 	equ 	0
CONIO 	equ 	1
;CONOUT equ 	2
CLKMODE	equ 	128
LED7SEG	equ 	129
;
;
; a  ... workspace reg.
; b  ... workspace reg. dnjz counter
; c  ... command
; de ... address
;

; rom subroutines;
;

        org     00100h
mon:            ;entry point
		ld 		de, 0
		ld 		(OADDR), de
		ld 		(CADDR), de
read_line:
		ld 		hl, lbuf 	; line buffer
		; ld 		c, BUFSIZE	; line buffer size (except null terminal)
		call	getln
		ld 		hl, lbuf
		ld 		a, (hl)
; no arity commands
		cp 		$0 		; line is empty
		jr 		z, default_dump
		;
		cp 		'H' 	; begins with H
		jp 		z, mon_halt
		;
		cp 		'.'		; begins with .
		jr 		z, specify_end
		cp 		':'		; begins with :
		jr 		z, write_mode
        cp      'S'
		;
; specify start address and function
		ld 		c, 4
		call 	hexstr_de
		ld 		(OADDR), de
		;
		ld 		a, (hl)
		cp 		$0
		jr 		z, default_dump
		cp		'.'
		jr 		z, specify_end
		cp		':'
		jr 		z, write_mode
		cp		'R'
		jr 		z, run_mode
		cp		'S'
		jr 		z, clk_mode
		call 	print_err_msg
		jr 		mon		; OADDR and CADDR are possibly corrupted
		;
specify_end:
		inc 	hl 		; next to '.'
		ld 		c, 4
		call 	hexstr_de
		ld 		(CADDR), de
		ld 		a, (hl)
		cp 		0
		jr 		z, default_dump
		call 	print_err_msg
		jr 		mon		; OADDR and CADDR are possibly corrupted
		;
default_dump:
		ld 		hl, (OADDR)
		ld 		de, (CADDR)
		ld 		a, h
		cp 		d 
		jr 		nz, $+4 
		ld 		a, l 
		cp 		e 
		jr 		c, do_dump  ; start < end
		jr 		z, do_dump  ; start == end
		ld 		de, $10
		add 	hl, de
		ld 		(CADDR), hl
		;
do_dump:
		ld 		hl, (OADDR)
		ld 		de, (CADDR)
		call 	dump
		ld 		(OADDR), hl
		ld 		de, 0
		ld 		(CADDR), de
		jp 		read_line
        ;
write_mode:
		inc 	hl 		; next to ':'
		ld 		a, (hl)
		cp 		' '
		jr 		z, write_mode
		;
		ld 		c, 2
		call 	hexstr_de
		ld 		a, c 
		cp 		a, 2
		jr 		z, write_mode.exit	; no arg or illegal char
		ld 		ix, (OADDR)
		ld 		(ix), e
		inc 	ix
		ld 		(OADDR), ix
		ld 		a, (hl)
		cp 		0
		jr 		z, write_mode.exit
		jr 		write_mode
write_mode.exit
		jp 		read_line
;
run_mode:
		ld 		hl, (OADDR)
		jp 		(hl)
;;
;  clockspeed change by output number to port 128
clk_mode:
		ld 		a, e
		call 	clk_spd_change
		jp 		read_line
;
mon_halt:
		halt


; subroutines
; getchar
getchar:
		in 		a, (CONSTA)
		and 	a
		jr 		z, getchar
		in 		a, (CONIO)
		ret

; getlin
; read up to c bytes into buffer pointed by hl, end with 0
; hl ... line buffer pointer
; c  .... buffer limit length
getln:
		ld 		(hl), 0
		call 	print_endl
		ld 		a, '*'
		out 	(CONIO), a
        ;
getln_wait:
		call 	getchar
		cp 		$08 	;backspace
		jr 		z, getln_bkspc
		cp 		$7f		; del
		jr 		z, getln_bkspc
		cp 		$1b
		jr 		z, getln_escseq
		cp 		$0a
		jr 		z, getln_end
		cp 		$0d
		jr 		z, getln_end
; other ctrl codes
		cp 		$20
		jr 		nc, getln_echo_proceed
		jr 		getln_wait

getln_bkspc:
		ld 		a, l
		and 	a
		jr 		z, getln_wait
		ld 		a, $08
		out 	(CONIO), a
		ld 		a, ' '
		out 	(CONIO), a
		ld 		a, $08
		out 	(CONIO), a
		dec 	l
		ld 		(hl), $0
		jr 		getln_wait

getln_escseq:
		call  	getchar
		cp 		'['
		jr 		nz, getln_wait
		call 	getchar
		cp 		'D'
		jr 		z, getln_bkspc
		jr 		getln_wait

getln_echo_proceed:
		out 	(CONIO), a 		; echo back
		;
		ld 		(hl),a		; *ptr++ = a
		inc 	l
		ld 		(hl), $0	; *ptr = NULL
		ld 		a, l
		cp 		$ff
		jr 		z, getln_end  ; force terminate line
		jr 		getln_wait

getln_end:	; parse lbuf
		ret

;
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
;
print_str_hl:
		ld 		a,(hl)
		and 	A
		ret 	z
		out 	(CONIO), a
		inc 	hl
		jr 		print_str_hl
;
;
print_err_msg:
		push 	hl
		push 	af
		call 	print_endl
		ld 		hl, str_err
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
str_err:
		db 	$0a, $0d, "error"
str_endl:
		db $0a, $0d, 0
;

; dump : dump memory from OADDR to OADDR+2 (value)
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

; convert one char expressing a hexadecimal digit 
; in A reg. to nibble in A
; set carry flag if got a wrong char
;
hex2nib:
		cp 		'0'
		ret 	c		; A < '0'
		cp 		'9' + 1
		jr 		nc, hex2nib.alpha
		sub 	'0' 	; A was digit, results carry reset
		ret
hex2nib.alpha:
		cp 		'a'
		jr 		c, hex2nib.upper
		cp 		'f'+1
		ccf		; set carry if A >= 'f'+1
		ret 	c
		and 	$df		; a - f to upper char
hex2nib.upper:
		cp 		'A'
		ret 	c
		cp      'F' + 1
		ccf
		ret 	c      ; error if it is larger than 'F'
		sub 	'A'-10
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
		ret 	c 			; encountered non-hexdec char.
hexstr_de.hex2nib_succ:
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
clk_spd_change:
		and 	$07
		out		(CLKMODE), a
		ret
;
; work space
			org 	0300h
workspace:
lbuf: 		ds 		128
BUFSIZE 	EQU 	127
OADDR:		DB		0, 0
CADDR:		DB		0, 0
STATE: 		DB 		0
;
