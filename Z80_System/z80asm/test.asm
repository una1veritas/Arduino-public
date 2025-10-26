;#include "lib_rom_mon.inc"

CONSTA  equ     $00
CONIO   equ     $01

        org     01000h

        ld      hl, crlf
        call    print_hl
        ld      b, 7
        ld      d, 2
start:  in      a, (CONSTA)
        and     a
        jr      z, start
        out     (CONIO), a
        ld      hl, startmsg
        call    print_hl

repeat: in      a, (CONIO)
        ld      c, a
        rlc     c
        rlc     c
        rlc     c
        rlc     c
        ld      a, $0f
        and     c
        add     '0'
        out     (CONIO), a
        rlc     c
        rlc     c
        rlc     c
        rlc     c
        ld      a, $0f
        and     c
        add     '0'
        out     (CONIO), a
        ld      hl, crlf
        call    print_hl
        djnz    repeat

        jp      0100h
        


print_hl:
        ld      a, (hl)
        and     a
        ret     z
        out     (CONIO), a
        inc     hl
        jr      print_hl

crlf:   db      $0d, $0a, 0
startmsg:
        db      "start", $0d, $0a, 0