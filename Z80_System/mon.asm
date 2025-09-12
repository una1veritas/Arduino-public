;
	org 	0000h
rst:
	ld 		sp, 0200h
    jp  	main

	org 	0010h
	ds 		10, $00
	ds 		$10, $00

	org 	0020h
main:
	ld 		hl, prompt
	call 	print_hl_str
	ld 		hl, inputbuffer
	ld 		b, 127
	call 	get_line
	ld 		hl, inputbuffer
	call 	print_hl_str
	call 	print_endl
	ld 		hl, inputbuffer
    call    read_hex
	ex 		de, hl
    call    print_hl_dec
	call 	print_endl

	jp 		main

	halt

get_line:
	in 		a, (0)
	and 	a
	jr 		z, get_line
	in 		a, (1)
	ld 		(hl), a
	cp 		10
	jr 		z, get_line_end
	cp 		13
	jr 		z, get_line_end
	inc 	hl
	djnz 	get_line
get_line_end:
	ld 		(hl), 0
get_line_clr_buf:
	in 		a, (0)
	and 	a
	ret 	z
	in 		a, (1)
	jr 		get_line_clr_buf


read_hex:
    ld      e, $00
    ld      d, $00

    ld      c, $04
loop:
    ld      a, (hl)
    cp      '0'
    jr      c, err
	cp      '9'+1
    jr 		c, one2nine
	cp 		'A'
	jr 		c, err
	cp		'F'+1
	jr 		c, a2f_lcap
	cp 		'a'
	jr		c, err
	cp 		'f'+1
	jr 		nc, err
a2f_scap:
	and 	$df
a2f_lcap:
	sub 	'A'+10
one2nine:
	sub		'0'

shift_in:
	ld 		b, 4
shift_in_loop:
    rl      e
    rl      d
    djnz    shift_in_loop
	add 	e
	ld 		e, a
    inc     hl
    dec     c
    jr      nz, loop

err:
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
	ds 		32,0