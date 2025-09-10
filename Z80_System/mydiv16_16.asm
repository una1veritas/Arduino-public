
    org     0000h
rst:
    ld      sp, 0100h
    jp      main

; working area
    org     0010h
dividiend:
    db  $3b, $00
divisor:
    db  $1c, $00
quotient:
    db  0, 0
reminder:
    db  0, 0

    ds  8, 0

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

    halt

; The following routine divides ac (dividiend) by de (divisor) and 
; places the quotient in ac and the remainder in hl
; destrys hl, a, b, c, d, e, ix
; https://wikiti.brandonw.net/index.php?title=Z80_Routines:Math:Division
;
div16:
    ld      hl, 02
    add     hl, sp
    ld      c, (hl)
    inc     hl
    ld      a, (hl)
    inc     hl
    ld      de, (hl)
div16_ac_de:
    ld	    hl, 0
    ld	    b, 16
div16_ac_de_loop:
    sll	c
    rla
    adc	hl, hl
    sbc	hl, de
    jr	nc, $+4 
    add	hl, de
    dec	c
    
    djnz	div16_ac_de_loop
div16_ac_de_end:
    ld      e, l
    ld      d, h
    ld      hl, 02
    add     hl, sp
    ld      (hl), c
    inc     hl
    ld      (hl), a
    inc     hl
    ld      (hl), de
    ret


; 0010 : 3b 00 1c 00 02 00 03 00 00 00 00 00 00 00 00 00 
; 0010 : 3b 00 1c 00 02 00 03 00 00 00 00 00 00 00 00 00 
; 00F0 : 00 00 00 00 00 00 00 00 02 00 2b 00 02 00 03 00

; 0010 : 50 30 c8 00 3d 00 a8 00 00 00 00 00 00 00 00 00

; 0031 : 19 33 ac 07 69 ba ab 04 
; 00F0 : 00 00 00 00 00 00 00 00 00 00 0e 00 69 ba ab 04
; 000e --> call (cd) + 3

;The following routine divides hl by c and places the quotient in hl and the remainder in a
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