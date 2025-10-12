#include "lib_rom_mon.inc"

CONSTA  equ     $00
CONIO   equ     $01

        org     0100h
vt_test:
        ld      hl , wrkspc
        call    print_str_hl
        call    print_endl
        jp      mon


        org 0120h
wrkspc:
        db $0a, $0d
        db  "This is an apple."
        db $0a, $0d
        db  $1b, "[H", $1b, "[2J"
        db "Hello, my friends!!!"
        ; db  $08, $08, $08, $07, $07
        db $1b, "[D", $1b, "[D", $1b, "[D", $1b, "[D", $1b, "[D"
        db $1b, "[K"
        db 0

        org  0140h
; ASCII CONTROL CODE
BACKSPACE:
        db  $08
BEEP:   db  $07
DELETE: db  $7f

; VT100 ESC sequences
;
CLRSCRN:
        db  $1b, "[H", $1b, "2J", 0
HOMEPOS:
        db  $1b, "[H", 0
CURSORL:
        db  $1b, "[D", 0
DELCHAR:
        db  $1b, "[P", 0
DELLEND:
        db  $1b, "[k", 0
INSTSPC:
        db  $1b, "[@", 0
INSTLIN:
        db  $1b, "[L", 0
