;
	org 	0000h
rst:
	ld 		sp, 0200h
    jp  	main

	ds 		10, $00
	ds 		$10, $00

	org 	0020h
main:
;	ld		hl, greetings
;	call 	print_hl_str
;	ld 		b, 19
;	call 	compute_fibo
;	ld 		hl, (var_b)
;	call 	print_hl_hex
;	call 	print_endl
;	ld 		a, $df 
;	call 	print_a_hex
;	call 	print_endl
;	ld 		hl, (var_b)
;	call 	print_hl_dec
;	call 	print_endl

	ld 		hl, prompt
	call 	print_hl_str
	call 	print_endl
	ld 		hl, inputbuffer
	ld 		b, 15
	call 	get_line
	ld 		hl, inputbuffer
	call 	print_hl_str
	call 	print_endl

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
	ld 	(hl), 0
	ret

;compute_fibo:
;	ld 		hl, 1
;	ld 		(var_a), hl
;	ld 		(var_b), hl
;	ld 		a, b
;	sub 	2
;	ret 	s
;	ret 	z
;	ld 		b, a
;compute_fibo_loop:
;	ld 		HL, (var_a)
;	ld 		d, h
;	ld 		e, l
;	ld 		HL, (var_b)
;	ld 		(var_a), hl
;	add 	hl, de
;	ld 		(var_b), hl
;	djnz	compute_fibo_loop
;	ret
;var_a:
;	db 	0,0
;var_b:
;	db 	0,0

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

;greetings:
;	db	"Hello, everyone!"
;	db  13, 10
;	db	0

prompt:
	db 	"type keys and then return."
	db 	0

inputbuffer:
	ds 		16,0