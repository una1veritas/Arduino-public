;
	    org 	0000h
RST_00:
	    ld 		sp, $2000
	    jp  	mon
;
;;
; a  ... workspace reg.
; b  ... workspace reg. dnjz counter
; c  ... command
; de ... address
; ix ...  mon_curr_addr, curr_addr + 2 == end_addr
;


; work space
addr	equ		$2000
addr2	equ 	addr+2
lbuf	equ 	addr2+2

        org     0040h
mon:            ;entry point
		ld 		de, 0
		ld 		(addr), de
		ld 		(addr2), de
		ld 		(lbuf), de
read_line:
		ld 		c, 31
		call	getln
		ld 		hl, lbuf
		ld 		a, (hl)
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
; specify start
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
		jr 		error
		;
specify_end:
		inc 	hl 		; next to '.'
		ld 		c, 4
		call 	hexstr_de
		ld 		(addr2), de
		ld 		a, (hl)
		cp 		0
		jr 		nz, error
		;
default_dump:
		ld 		hl, (addr)
		ld 		de, (addr2)
		ld 		a, h
		cp 		d
		jr 		nz, cp_hl_de_end
		ld 		a, l
		cp 		e
cp_hl_de_end:
		jr 		z, do_dump 	; =
		jr 		c, do_dump	; <
		;jr 		nc, cp_gt 	; >
;cp_gt:
		ld 		hl, (addr)
		ld 		de, $10
		add 	hl, de
		ld 		(addr2), hl
		;
do_dump:
		call 	dump
		jr 		read_line
;
write_mode:
		inc 	hl 		; next to ':'
		ld 		a, (hl)
		cp 		' '
		jr 		z, write_mode
		;
		ld 		c, 2
		call 	hexstr_de
		ld 		a,c 
		cp 		a, 2
		jr 		z, write_mode.exit	; no arg or illegal char
		;call 	print_endl
		;ld 		a, e
		;call print_byte
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
;
error:
		call 	print_endl
		call 	print_byte
		ld 		hl, err_msg
		call 	print_str_hl
		ld 		hl, lbuf
		call 	print_str_hl
		jp 		read_line

err_msg:
		db 	" error", $0a, $0d, 0

mon_halt:
		halt


; バッファ方式にしてるから最大4ニブルを一気に読んだ方がかんたんでは？


; subroutines
; read up to c bytes into buffer pointed by hl, end with 0
getln:
		ld 		hl, lbuf 	; buf ptr
		ld 		(hl), 0
		ld 		b, 0		; char count
		call 	print_endl
		ld 		a, '*'
		out 	(2), a

getln_wait:
		in 		a, (0)
		and 	a
		jr 		z, getln_wait
;
; no echo back
		in 		a, (1)
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
		out 	(2), a
		ld 		a, ' '
		out 	(2), a
		ld 		a, $08
		out 	(2), a
		dec 	hl
		ld 		(hl), $0
		dec 	b
		jr 		getln_wait

getln_echo_proceed:
		out 	(2), a 		; echo back
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
; bit 7 is set to A if A is not hex-dec char
;
hex2nib:
		cp 		'0'
		jr 		c, hex2nib.err
		cp 		'9' + 1
		jr 		nc, hex2nib.toupper
		sub 	'0'
		ret
		;
hex2nib.toupper:
		cp 		'a'
		jr	 	c, hex2nib.alpha
		cp 		'f' + 1
		jr	 	nc, hex2nib.err
		sub 	$20 	; to upper char
hex2nib.alpha:
		sub     'A' 
		jr      c, hex2nib.err  
		cp      6
		jr      nc, hex2nib.err      ; error if it is larger than 'F'
		add 	10
		ret
		;
hex2nib.err:
		ld 		a, $ff 	; error code
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
		ld 		b, a
		call 	hex2nib
		cp 		$ff
		jr 		nz, hexstr_de.hex2nib_succ
		ld 		a, b 	; recover original value of A
		ret 	 		; encountered non-hexdec char.
hexstr_de.hex2nib_succ:
		and 	a		; clear Carry bit
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
		out 	(2), a
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
		ld 		a, $0d
		out 	(2), A
		ret

; dump memory from addr to addr+2 (value)
;
dump:
	ld 		hl, (addr)
	ld 		de, (addr2)
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
	ld 		b, 16 	; up to 16 bytes
dump_16:
	ld 		a, (hl)
	inc 	hl
	ld 		(addr), hl
	call 	print_byte
	ld 		a, ' '
	out 	(2), a
cp_de_hl:
	ld 		a, d
	cp 		h
	jr 		nz, cp_de_hl.comp_end
	ld 		a, e
	cp 		l
cp_de_hl.comp_end:
	jr 		z, dump_exit
	jr 		c, dump_exit
;
	djnz 	dump_16
	jr 		dump_header

dump_exit:
	;call 	print_endl
	ld 		de, 0
	ld 		(addr2), de
	ret
