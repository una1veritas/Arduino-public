; dma test 0080
            org     1000h
DMAL        equ      15
;
            ld      hl, FILENAME
            ld      bc, DMAL
            ld      a, l
            out     (c), a
            inc     c  ; DMAH
            ld      a, h
            out     (c), a 
            inc     c  ; DMABLKSIZE
            ld      a, 0
            out     (c), a
            inc     c   ; DMAEXEC
            ld      a, 1
            out     (c), a  ; exec dma from ram to buffer
            inc     c       ; DMARES
            in      a, (c)
            ld      (DMARES), a
            jp      0100h


DMARES:     db      0
FILENAME:   db      "EMUZ80BASIC.rom", 0

            end
