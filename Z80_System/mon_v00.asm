;
	org 	0000h
rst:
	ld 		sp, 0200h
	jp  	main
;
;;
; a  ... working reg.
; b  ... dnjz counter
; c  ... nibble value
; de ... start address
; hl ... end address
;
	org 	0010h
	ld 		hl, mon_cmd
	ld 		(hl), 0
main:
	ld 		a, '>'
	out 	(2), a
	ld 		a, ' '
	out 	(2), a
wait_next_char:
	call 	getchar
	out 	(2), a 		; echo back

	call 	hex2nibble
	cp 		$ff
	jr 		z, cmd
	ld 		c, a
shift_in:
	rl		c
	rl		c
	rl		c
	rl		c
	ld 		b, 4
_loop:
	rl 		c
	rl		e
	rl 		d
	djnz 	_loop
	ld 		hl, mon_curr_addr
	ld 		(hl), de
	jr 		wait_next_char

cmd:
	ld 		a, c
	cp 		'H'
	jr 		nz, next_cmd1

	halt

next_cmd1:
	cp 		'R'
	jr 		nz, next_cmd2
	push 	de
	ret

next_cmd2:
	cp 		'.'
	jr 		nz, next3
	ld 		hl, mon_cmd
	ld 		(hl), a

next3:
	cp 		$0d
	jr 		z, exec_command

next4:
	jr 		main

exec_command:
	call 	dump_16_byte
	jr 		main

; dump memory b bytes from address stored in addrptr
dump_16_byte:
	ld 		hl, (mon_curr_addr)
dump_16_byte_loop:
	ld 		a, h
	call 	print_a_hex
	ld 		a, l
	call 	print_a_hex
	ld 		a, ' '
	out 	(2), a
	ld 		a, ':'
	out 	(2), a
	ld 		a, ' '
	out 	(2), a
	ld 		b, 16
dump_16:
	ld 		a, (hl)
	call 	print_a_hex
	ld 		a, ' '
	out 	(2), a
	inc 	hl
	djnz 	dump_16
dump_16_byte_exit:
	ld 		(mon_curr_addr), hl
	call 	print_endl;
	ret

; read one character from con in 
; returns ascii code in A reg.
getchar:
	in 		a, (0)
	and 	a 
	jr 		z, getchar
	in 		a, (1)
	ret

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



print_nibble_hex:
	and 	$0f
	add 	'0'
	cp 		':'
	jr 		c, print_nibble_hex_out
	add 	7
print_nibble_hex_out:
	out 	(2), a
	ret

print_a_hex:
	push 	af
	rlca
	rlca
	rlca
	rlca
	call 	print_nibble_hex
	pop 	af
	call 	print_nibble_hex
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


	org 	0100h
; monitor working area
prompt:
	db 	"> "
	db 	0

mon_cmd:
	db 		$0
mon_curr_addr:
	dw 		0000h
