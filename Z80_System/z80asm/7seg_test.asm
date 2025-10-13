#include "lib_rom_mon.inc"

        org     0100h
test:
        call    led7seg.out
        jp      mon

led7seg.out:
        ld      hl, LED7SEG_DATA
        ld      b, 8
loop:   ld      a, (hl)
        out     (LED7SEG), A
        inc     hl
        djnz    loop
        ret

        org     0120h
LED7SEG_DATA:
        db      "ABCD0123"