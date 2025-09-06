        org     0000h
init:
        ld      sp, 0200h
start:
        ld      hl, (divisor)
        ld      bc, (dividiend)
        call    cp16_hl_bc
        jp      z, case_eq
        jp      c, case_less
        jp      case_more
        halt
case_eq:
        ld      hl, msg_eq
        call    print_str
        jp      next
case_more:
        ld      hl, msg_more
        call    print_str
        jp      next
case_less:
        ld      hl, msg_less
        call    print_str
next:
        ld      hl, (divisor)
        inc     hl
        ld      (divisor), hl
        jp      start

cp16_hl_bc:
        ld      a, h
        cp     b
        jr      nz, .done
        ld      a, l
        cp      c
.done:
        ret

print_str:
	    ld	    a,(hl)
	    and	    a
	    ret	    z
	    out	    (2),a
	    inc	    hl
	    jr	    print_str

msg_eq:
        db      "are equal."
        db      10, 13, 0
msg_more:
        db      "is larger."
        db      10, 13, 0
msg_less:
        db      "is smaller.."
        db      10, 13, 0


dividiend:
        db      $03, $21
divisor:
        db      $fa, $20
quotient:
        db      0, 0
reminder:
        db      0, 0
