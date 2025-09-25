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
