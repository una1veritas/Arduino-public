
    org     0000h
rst:
    ld      sp, 0100h
    jp      main

; working area
    org     0010h
dividiend:
    dw      $e6b3 
divisor:
    dw      $0085
quotient:
    dw      0
reminder:
    dw      0

    ds      8, 0

    org     0020h
main:
    ld      hl, (divisor)
    push    hl
    ld      hl, (dividiend)
    push    hl
    call    div16
    pop     hl
    ld      (quotient), hl
    pop     hl
    ld      (reminder), hl

    ld      hl, (reminder)
    push    hl
    ld      hl, (quotient)
    push    hl
    call    div16_8
    pop     hl
    ld      (quotient+4), hl
    pop     hl
    ld      (reminder+4), hl
    halt

; divide dividiend in d by divisor in e 
; returns quotient in d and remainder in a
div_d_e:
   xor	a
   ld	b, 8

div_d_e_loop:
   sla	d
   rla
   cp	e
   jr	c, $+4
   sub	e
   inc	d
   
   djnz	div_d_e_loop
   
   ret
   
; dvi_hl_de stack in out wrapper
; [ret addr] [dividiend] [divisor] 
; --> [ret addr] [quotient] [reminder]
div16:
    ld      ix, 02
    add     ix, sp
    ld      c, (ix)
    ld      a, (ix+1)
    ld      de, (ix+2)
    call    div_ac_de
    ld      (ix+2), hl
    ld      (ix), c
    ld      (ix+1), a
    ret

; The following routine divides ac (dividiend) by de (divisor) and 
; places the quotient in ac and the remainder in hl
; destrys hl, a, b, c, d, e, ix
; https://wikiti.brandonw.net/index.php?title=Z80_Routines:Math:Division
;
div_ac_de:
    ld	    hl, 0
    ld	    b, 16
div_ac_de_loop:
    sll	c
    rla
    adc	hl, hl
    sbc	hl, de
    jr	nc, $+4 
    add	hl, de
    dec	c
    
    djnz	div_ac_de_loop
    ret




; dvi_hl_c stack in out wrapper
; [ret addr] [dividiend] [divisor:low] 
; --> [ret addr] [quotient] [reminder:low]
div16_8:
    ld      ix, 02
    add     ix, sp
    ld      hl, (ix)
    ld      c, (ix+2)
    call    div_hl_c
    ld      (ix), hl
    ld      (ix+2), a
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


; print the decimal integer in HL 
print_hl_dec:
	xor 	a
	push 	af 		; terminal null char
	ld 		c, 10 	; radix = 10

print_hl_dec_loop0:
	call 	div_hl_c 	; a = hl % 10, hl = hl / 10
	add 	a, $30		; to ascii code between '0' and '9'
	push 	af
	ld 		a, h 		; check whether hl is empty
	or 		l
	jr 		z, print_hl_dec_output		; if z then conversion finished
	jr 		print_hl_dec_loop0

print_hl_dec_output:
	pop 	af
	and 	a
	ret 	z
	out 	(2), a
	jr 		print_hl_dec_output
