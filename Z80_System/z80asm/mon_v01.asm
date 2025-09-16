;
	org 	0000h
rst:
	ld 		sp, 0150h
	jp  	monitor
;
;;
; a  ... workspace reg.
; b  ... workspace reg. dnjz counter
; c  ... command
; de ... address
; ix ...  mon_curr_addr, curr_addr + 2 == end_addr
;

MODE_NOITEM		equ 	0
MODE_HASITTEM 	equ		1
MODE_ITTEM_BIT 	equ		0
MODE_RANGE		equ 	4
MODE_RANGE_BIT	equ 	2
MODE_WRITE		equ 	8
MODE_WRITE_BIT	equ 	3

	org 	0010h
monitor:
	ld 		ix, mon_curr_addr
monitor_init:
	ld      c, MODE_NOITEM
    ld      de, 0
monitor_main:
	call 	print_endl
	ld 		a, '*'
	out 	(2), a
next_char:
	call 	getchar
	out 	(2), a 		; echo back (including line end)
;
	cp 		$0d
	jr 		z, exec_cmd 	; line ended
	cp 		$0a
	jr 		z, exec_cmd 	; line ended

; single char command
    cp      'H'
    jr      z, mon_halt
; single char command
    cp      'R'
    jr      z, mon_reset

; mode command
	cp 		'.'
	jr 		nz, skip_mode_read
	set 	MODE_RANGE_BIT, c
skip_mode_read:

	cp 		':'
	jr 		nz, skip_mode_write
	set 	MODE_WRITE_BIT, c
skip_mode_write:

	call 	hex2nibble
	cp 		a, $ff
	jr 		z, next_char 	; seems got error by unexpected char
;
; shift DE 4 bit and add the nibble in A 
	ld 		b, 4
_rl_4
	and 	a		; clr carry
	rl		e
	rl 		d
	djnz 	_rl_4
	add 	e
	ld 		e, a
;
	set		MODE_ITTEM_BIT, c

	bit 	MODE_RANGE_BIT, c
	jr 		nz, __second_item
	bit 	MODE_WRITE_BIT, c
	jr 		nz, __second_item
	ld 		(ix), de
    jr  	next_char
__second_item:
	ld 		(ix+2), de
	jr 		next_char

mon_halt:
mon_reset:
    halt

exec_cmd:
	call 	dump
	jr 		monitor_init



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

; dump memory b bytes from address in hl
dump:
	ld 		hl, (ix)
dump_header:
	call 	print_endl
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
;
	ld 		b, 16
dump_16:
	ld 		a, (hl)
	call 	print_byte_hex
	ld 		a, ' '
	out 	(2), a
	inc 	hl
;
	ld 		(ix), hl
	ld	 	de, (ix+2)
	and 	a
	sbc 	hl ,de
	ld		hl, (ix)
	jr 		z, dump_exit
	jp 		s, dump_exit
	djnz 	dump_16
	jr 		dump_header

dump_exit:
	ld 		de, 16
	add 	hl, de
	ld 		(ix+2), hl
	ret

	org		0120h
mon_curr_addr:
	dw		0000h
mon_end_addr:
	dw		0000h
