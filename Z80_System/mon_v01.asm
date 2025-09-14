;
	org 	0000h
rst:
	ld 		sp, 0200h
	jp  	monitor
;
;;
; a  ... working reg.
; b  ... dnjz counter
; c  ... command
; de ... address
;
	org 	0010h
monitor:
	ld      c, 0
	ld 		ix, mon_curr_addr
    ld      de, (ix)
monitor_main:
	call 	print_endl
	ld 		a, '*'
	out 	(2), a
	ld 		a, ' '
	out 	(2), a
next_char:
	call 	getchar
	cp 		$0d
	jr 		z, next_char
	cp 		$0a
	jr 		z, exec_cmd
echo_back:
	out 	(2), a 		; echo back (including cr)
    cp      'H'
    jr      z, mon_halt

	call 	hex2nibble
	cp 		a, $ff
	jr 		z, next_char 	; delimiter or other error char
	ld 		b, 4
_rl_4
	and 	a		; clr carry
	rl		e
	rl 		d
	djnz 	_rl_4
	add 	e
	ld 		e, a
    jr  	next_char

mon_halt
    halt

exec_cmd:
	call 	print_endl
	ld 		ix, mon_curr_addr
	ld 		(ix), de
	ld 		hl, (ix)
	call 	dump
	ld 		(ix), hl
	jr 		monitor



; read one character from con in 
; returns ascii code in A reg.
getchar:
	in 		a, (0)
	and 	a 
	jr 		z, getchar
	in 		a, (1)
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


; print a nibble in A
print_nibble_hex:
	and 	$0f
	add 	'0'
	cp 		':'
	jr 		c, print_nibble_hex_out
	add 	7
print_nibble_hex_out:
	out 	(2), a
	ret

; print a byte in A
print_byte_hex:
	push 	af
	rlca
	rlca
	rlca
	rlca
	call 	print_nibble_hex
	pop 	af
	call 	print_nibble_hex
	ret

print_endl:
	ld 	a, $0a
	out 	(2), A
	;ld 		a, $0d
	;out 	(2), A
	ret

; dump memory b bytes from address stored in addrptr
dump:
	ld 		a, h
	call 	print_byte_hex
	ld 		a, l
	call 	print_byte_hex
	ld 		a, ' '
	out 	(2), a
	ld 		a, ':'
	out 	(2), a
	ld 		a, ' '
	out 	(2), a
	ld 		b, 16
dump_16:
	ld 		a, (hl)
	call 	print_byte_hex
	ld 		a, ' '
	out 	(2), a
	inc 	hl
	djnz 	dump_16
dump_exit:
	; call 	print_endl;
	ret

mon_curr_addr:
	dw		0000h
