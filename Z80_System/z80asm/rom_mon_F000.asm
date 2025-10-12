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
BUSDISP	equ 	129
;
;
; a  ... workspace reg.
; b  ... workspace reg. dnjz counter
; c  ... command
; de ... address
; ix ...  mon_curr_addr, curr_addr + 2 == end_addr
;

; rom subroutines;
;clk_spd_change     = $F11D
;dump               = $F09F
;getln              = $F000 
;hex2nib            = $F0E8 
;hexstr_de          = $F106 
;print_byte         = $F05B 
;print_endl         = $F068 
;print_err_msg      = $F079 
;print_nibble       = $F04C 
;print_str_hl       = $F071 
;

; work space
addr	equ		0EFA0H
addr2	equ 	addr+2
escqbuf	equ		addr+4
lbuf	equ 	0EFB0H
BUFSIZE equ 	63

;

        org     0F000h
mon:            ;entry point
		ld 		de, 0
		ld 		(addr), de
		ld 		(addr2), de
		ld 		(lbuf), de 
read_line:
		ld 		hl, lbuf 	; line buffer
		ld 		c, BUFSIZE	; line size (except null terminal)
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
		;
; specify start address and function
		ld 		c, 4
		call 	hexstr_de
		ld 		(addr), de
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
		jr 		mon		; addr and addr2 are possibly corrupted
		;
specify_end:
		inc 	hl 		; next to '.'
		ld 		c, 4
		call 	hexstr_de
		ld 		(addr2), de
		ld 		a, (hl)
		cp 		0
		jr 		z, default_dump
		call 	print_err_msg
		jr 		mon		; addr and addr2 are possibly corrupted
		;
default_dump:
		ld 		hl, (addr)
		ld 		de, (addr2)
		ld 		a, h
		cp 		d 
		jr 		nz, $+4 
		ld 		a, l 
		cp 		e 
		jr 		c, do_dump  ; start < end
		jr 		z, do_dump  ; start == end
		ld 		de, $10
		add 	hl, de
		ld 		(addr2), hl
		;
do_dump:
		ld 		hl, (addr)
		ld 		de, (addr2)
		call 	dump
		ld 		(addr), hl
		ld 		de, 0
		ld 		(addr2), de
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
		ld 		ix, (addr)
		ld 		(ix), e
		inc 	ix
		ld 		(addr), ix
		ld 		a, (hl)
		cp 		0
		jr 		z, write_mode.exit
		jr 		write_mode
write_mode.exit
		jp 		read_line
;
run_mode:
		ld 		hl, (addr)
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
;
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
		ld 		b, 0		; char count
		call 	print_endl
		ld 		a, '*'
		out 	(CONIO), a
        ;
getln_wait:
		call 	getchar
		;
		cp 		$08 	;backspace
		jr 		z, getln_bkspc
		cp 		$7f		; del
		jr 		z, getln_bkspc
		cp 		$0a
		jr 		z, getln_end
		cp 		$0d
		jr 		z, getln_end
; other ctrl codes
		cp 		$1b
		jp 		z, esc_seq_machine
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

esc_seq_machine:
		ld		ix, escqbuf
		ld 		(ix), a
		inc 	ix
		call 	getchar
		cp 		'['
		jr 		nz, getln_wait
		ld 		(ix), a
		inc 	ix
		call 	getchar
		cp 		'D'
		ret 	nz
		ld 		(ix), a
		inc 	ix
		clra	
		ld 		(ix), a
		; echo back
		jr 		getln_bkspc

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
;
; translate adcii char digit in A register
; into integer value
dec2nib:
		cp 		'0'
		ret 	c		; A < '0'
		cp 		'9' + 1
		ccf
		ret 	c
		sub 	'0' 	; A was digit, results carry reset
		ret
;
;




;
clk_spd_change:
		and 	$07
		out		(CLKMODE), a
		ret

;
; arithmetic routines
;
;8*8 multiplication
;The following routine multiplies h by e and places the result in hl
mul_h_e:
   ld	d, 0	; Combining the overhead and
   sla	h		; optimised first iteration
   sbc	a, a
   and	e
   ld	l, a
   ;
   ld	b, 7
mul_h_e.loop:
   add	hl, hl          
   jr	nc, $+3
   add	hl, de
   ;
   djnz	mul_h_e.loop
   ;
   ret

; 16*8 multiplication
; The following routine multiplies de by a and places the result in ahl
; (which means a is the most significant byte of the product, l the least 
; significant and h the intermediate one...)
;
mul_a_de:
   ld	c, 0
   ld	h, c
   ld	l, h
   ;
   add	a, a		; optimised 1st iteration
   jr	nc, $+4
   ld	h,d
   ld	l,e
   ;
   ld b, 7
mul_a_de.loop:
   add	hl, hl
   rla
   jr	nc, $+4
   add	hl, de
   adc	a, c   		; yes this is actually adc a, 0 but since c is free we set it to zero and so we can save 1 byte and up to 3 T-states per iteration
   ;
   djnz	_loop
   ;   
   ret

;
; 16*16 multiplication
;The following routine multiplies bc by de and places the result in dehl.
mul_de_bc:
   ld	hl, 0

   sla	e		; optimised 1st iteration
   rl	d
   jr	nc, $+4
   ld	h, b
   ld	l, c

   ld	a, 15
_loop:
   add	hl, hl
   rl	e
   rl	d
   jr	nc, $+6
   add	hl, bc
   jr	nc, $+3
   inc	de
   
   dec	a
   jr	nz, _loop
   
   ret
 
 
; divide dividiend in d by divisor in e 
; returns quotient in d and remainder in a
div_d_e:
   xor	a
   ld	b, 8
   ;
div_d_e_loop:
   sla	d
   rla
   cp	e
   jr	c, $+4
   sub	e
   inc	d
   ;
   djnz	div_d_e_loop
   ;
   ret

; dvi_hl_de stack in out wrapper
; [ret addr] [dividiend] [divisor] 
; --> [ret addr] [quotient] [reminder]
div16:
    ld      ix, 02
    add     ix, sp
    ld      c, (ix)
    ld      a, (ix+1)
    ld      de, (ix+2)
    call    div_ac_de
    ld      (ix+2), hl
    ld      (ix), c
    ld      (ix+1), a
    ret

; The following routine divides ac (dividiend) by de (divisor) and 
; places the quotient in ac and the remainder in hl
; destrys hl, a, b, c, d, e, ix
; https://wikiti.brandonw.net/index.php?title=Z80_Routines:Math:Division
;
div_ac_de:
    ld	    hl, 0
    ld	    b, 16
div_ac_de_loop:
    sll	c
    rla
    adc	hl, hl
    sbc	hl, de
    jr	nc, $+4 
    add	hl, de
    dec	c
    
    djnz	div_ac_de_loop
    ret


; dvi_hl_c stack in out wrapper
; [ret addr] [dividiend] [divisor:low] 
; --> [ret addr] [quotient] [reminder:low]
div16_8:
    ld      ix, 02
    add     ix, sp
    ld      hl, (ix)
    ld      c, (ix+2)
    call    div_hl_c
    ld      (ix), hl
    ld      (ix+2), a
    ret

; divides hl by c and places the quotient in hl 
; and the remainder in a
div_hl_c:
   xor	a
   ld	b, 16

div_hl_c_loop:
   add	hl, hl
   rla
   jr	c, $+5
   cp	c
   jr	c, $+4

   sub	c
   inc	l
   
   djnz	div_hl_c_loop
   
   ret


; print the decimal integer in HL 
print_hl_dec:
	xor 	a
	push 	af 		; terminal null char
	ld 		c, 10 	; radix = 10

print_hl_dec_loop0:
	call 	div_hl_c 	; a = hl % 10, hl = hl / 10
	add 	a, $30		; to ascii code between '0' and '9'
	push 	af
	ld 		a, h 		; check whether hl is empty
	or 		l
	jr 		z, print_hl_dec_output		; if z then conversion finished
	jr 		print_hl_dec_loop0

print_hl_dec_output:
	pop 	af
	and 	a
	ret 	z
	out 	(2), a
	jr 		print_hl_dec_output
