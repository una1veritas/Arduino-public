; macros
clrcf:	macro		; clear carry flag
		and 	a
		endm
;
clra: 	macro
		xor 	a
		endm
;
;
	    org 	0000h
RST_00:
	    ld 		sp, $e000
	    jp  	mon
;
;;
; a  ... workspace reg.
; b  ... workspace reg. dnjz counter
; c  ... command
; de ... address
; ix ...  mon_curr_addr, curr_addr + 2 == end_addr
;
; I/O port
CONST 	equ 	$00
CONIO 	equ 	$01
;CONOUT equ 	$02
CLKMODE	equ 	$80

; rom subroutines;
clk_spd_change     = $F11D
dump               = $F09F
getln              = $F000 
hex2nib            = $F0E8 
hexstr_de          = $F106 
print_byte         = $F05B 
print_endl         = $F068 
print_err_msg      = $F079 
print_nibble       = $F04C 
print_str_hl       = $F071 
;
;

; work space
addr	equ		$e000
addr2	equ 	addr+2
lbuf	equ 	addr2+2
BUFSIZE equ 	63

        org     0040h
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
