;
	org 	0000h
rst:
	ld 		sp, 0200h
	jp  	main
;
;;
; a  ... working reg.
; b  ... dnjz counter
; c  ... command
; de ... address
;
	org 	0010h
	ld      c, 0
    ld      de, 0
main:
	ld 		a, '>'
	out 	(2), a
	ld 		a, ' '
	out 	(2), a
next_char:
	call 	getchar
	out 	(2), a 		; echo back
    cp      $0a
    jr      z, mon_halt
    cp  $0d
    jr  z, mon_halt
    jr  next_char

mon_halt
    halt

; read one character from con in 
; returns ascii code in A reg.
getchar:
	in 		a, (0)
	and 	a 
	jr 		z, getchar
	in 		a, (1)
	ret