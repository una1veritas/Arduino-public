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
