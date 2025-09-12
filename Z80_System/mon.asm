;
	org 	0000h
rst:
	ld 		sp, 0200h
	jp  	main

	org 	0010h
main:
	ld 		hl, prompt
	call 	print_hl_str
	ld 		hl, inputbuffer
	ld 		b, 127
	call 	getline
	ld 		hl, inputbuffer
	call 	print_hl_str
	call 	print_endl
	ld 		hl, inputbuffer
	call    hexstr4_de
	ld 		hl, de
	call    print_hl_dec
	call 	print_endl

	jp 		main

	halt

; read and store line until the 1st occurrence of cr or nl
; in (hl)
getline:
	in 		a, (0)		; con status
	and 	a			; 
	jr 		z, getline 	; repeat until available
	in 		a, (1)		; getchar
	ld 		(hl), a		; store
	cp 		10			; if nl
	jr 		z, getline_end
	cp 		13			; if cr
	jr 		z, getline_end
	inc 	hl			; ++hl
	djnz 	getline		; get next char
getline_end:
	ld 		(hl), 0		; terminate by null
getline_clrbuf:
	in 		a, (0)		; waste remaining chars in con buffer
	and 	a
	ret 	z
	in 		a, (1)
	jr 		getline_clrbuf


; read hexadecimal string upto 2 or 4 (set in c) 
; bytes from (hl) and return integer val in de
;
hexstr2_de:
	ld 		c, 2
	jr 		hexstr_de

hexstr4_de:
	ld 		c, 4
	jr 		hexstr_de

hexstr_de:
    ld      de, 0000h
hexstr_de_lp:
    ld      a, (hl)
	call 	hex2nibble
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

;convert one hexadecimal char in a to nibble in a 
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



; print the decimal integer in HL 
print_hl_dec:
	ld 		ix, 0
	push 	ix  ; secure 6 bytes (null + 5 digits for 16 bit)
	push 	ix
	push 	ix
	add 	ix, sp
	ld 		c, 10 	; radix = 10
print_hl_dec_loop0:
	inc 	ix  ; at first, skip the place for terminal null char 
	call 	div_hl_c 	; a = hl % 10, hl = hl / 10
	add 	a, $30		; to ascii code '0' to '9'
	ld 		(ix), a		; ix stays on the last written char
	ld 		a, h 		; are there bits remained in hl?
	or 		l
	jr 		z, print_hl_dec_output		; if conversion finished
	jr 		print_hl_dec_loop0
print_hl_dec_output:
	ld 		a, (ix)
	and 	a
	jr 		z, print_hl_dec_exit
	out 	(2), a
	dec 	ix
	jr 		print_hl_dec_output
print_hl_dec_exit:
	pop 	ix 
	pop 	ix 
	pop 	ix 
	ret


print_a_hex:
	ld 		h, a
	ld 		c, 2
	jr 		print_hl_hex_loop

print_hl_hex:
	ld 		c, 4
print_hl_hex_loop:
	call 	rotleft_ahl_4
	add 	'0'
	cp 		':'
	jr 		c, print_hl_hex_loop_out
	add 	7
print_hl_hex_loop_out:
	out 	(2), a
	dec 	c
	jr 		nz, print_hl_hex_loop
	ret

rotleft_ahl_4:
	ld 	b, 4
	xor a
rotleft_ahl_4_loop:
	rl 	l
	rl 	h
	rla
	djnz rotleft_ahl_4_loop
	ret

print_hl_str:
	ld	a, (HL)
	or	a
	ret	z
	out	(2),a
	inc HL
	jp	print_hl_str

print_endl:
	ld 		a, 13
	out		(2), a
	ld 		a, 10
	out 	(2), a
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


prompt:
	db 	"> "
	db 	0

inputbuffer:
	ds 		16,0