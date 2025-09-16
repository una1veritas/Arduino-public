;
	    org 	0000h
rst:
	    ld 		sp, 1000h
	    jp  	mon
;
;;
; a  ... workspace reg.
; b  ... workspace reg. dnjz counter
; c  ... command
; de ... address
; ix ...  mon_curr_addr, curr_addr + 2 == end_addr
;
		org 	0080h
addr:	dw		$0
valu:	dw 		$0	
lbuf:	ds 		32, 0

        org     00a0h
mon:            ;entry point

; やっぱり、一行よむバッファがないと、delete/backspace も
; 処理できないから、getline タイプに改める。
; そうしないと、del/backspace で de レジスタを逆転するだけじゃなく、
; . や : を跨いでもどさないといけないし

getln:
		ld 		hl, lbuf 	; buf ptr
		ld 		(hl), 0
		ld 		b, 0		; char count
getln_loop:
		in 		a, (0)
		and 	a
		jr 		z, getln_loop
		;
		in 		a, (1)
		cp 		$08 	;backspace
		jr 		z, getln_bkspc
		cp 		$7f		; del
		jr 		z, getln_bkspc
		jr 		non_del

getln_bkspc:
		ld 		a, b
		and 	a
		jr 		z, getln_loop
		ld 		a, $08
		out 	(2), a
		ld 		a, ' '
		out 	(2), A
		ld 		a, $08
		out 	(2), a
		dec 	hl
		ld 		(hl), $0
		dec 	b
		jr 		getln_loop

non_del:
		out 	(2), a 		; echo back
		
		cp 		$0d
		jr 		z, line_ended
		cp 		$0a
		jr 		z, line_ended

		ld 		(hl),a
		inc 	hl
		ld 		(hl), $0
		inc 	b
		ld 		a, b
		cp 		31
		jr 		z, getln
		jr 		getln_loop

line_ended:
		call 	print_endl
		ld 		hl, lbuf
		call 	print_str_hl
		call 	print_endl
		jr 		getln


print_str_hl:
		ld 		a,(hl)
		and 	A
		ret 	z
		out 	(2), a
		inc 	hl
		jr 		print_str_hl

print_endl:
		ld 		a, $0a
		out 	(2), A
		ld 	a, $0d
		out 	(2), A
		ret
