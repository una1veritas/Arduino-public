#include "lib_rom_mon.inc"

CONSTA  equ     $00
CONIO   equ     $01

        org     01000h
vt_test:
        ld      hl , wrkspc
        call    print_str_hl
        call    print_endl
        jp      mon


        org 01020h
wrkspc:
        db $0a, $0d

        db "Hello, my friends!!!"
        ; db  $08, $08, $08, $07, $07
        db $1b, "[D", $1b, "[D", $1b, "[D", $1b, "[D", $1b, "[D"
        db  $1b, "[P", $1b, "[P"
        db 0

        org  01050h
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
        db  $1b, "[D"
DELCHAR:
        db  $1b, "[P"
DELLEND:
        db  $1b, "[k"
INSTSPC:
        db  $1b, "[@"
INSTLIN:
        db  $1b, "[L"
