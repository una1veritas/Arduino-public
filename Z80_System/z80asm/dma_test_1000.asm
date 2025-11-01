; dma test 0080
            org     1000h
;
CONSTA      equ     0
CONIO       equ     1
DMAL        equ     15
XSTREAMST   equ     24
XSTREAMDAT  equ     25
;
            ld      hl, FILENAME
_xout_loop:
            ld      a, (hl)
            out     (XSTREAMDAT), a
            and     a
            jr      z, _xin_loop
            out     (CONIO), a
            inc     hl
            jr      _xout_loop
_xin_loop:
            in      a,  (XSTREAMDAT)
            and     a
            jr      z, _ending
            out     (CONIO), a
            inc     hl
            jr      _xin_loop
_ending:
            jp      0100h

DMARES:     db      0
FILENAME:   db      "EMUZ80BASIC.rom"
CRLF:       db      13, 10, 0
            end
