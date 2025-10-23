;  The WOZ Monitor for the Apple 1
;  Written by Steve Wozniak in 1976


; Page 0 Variables
; + $100

XAML            = $24 + $100           ;  Last "opened" location Low
XAMH            = $25 + $100          ;  Last "opened" location High
STL             = $26 + $100          ;  Store address Low
STH             = $27 + $100          ;  Store address High
L               = $28 + $100          ;  Hex value parsing Low
H               = $29 + $100          ;  Hex value parsing High
YSAV            = $2A + $100          ;  Used to see if hex value is given
MODE            = $2B + $100          ;  $00=XAM, $7F=STOR, $AE=BLOCK XAM


; Other Variables

;IN              = $0200         ;  Input buffer to $027F
IN_BUFF          = $200
;KBD             = $D010         ;  PIA.A keyboard input
;KBDCR           ; ??= $D011         ;  PIA.A keyboard control register
;DSP             = CONIO         ; $D012         ;  PIA.B display output register
;DSPCR           ; ??= $D013         ;  PIA.B display control register

               .org     $fa00  ;$FF00
               .export  RESET

RESET:          ; CLD       N.A.      ; Clear decimal arithmetic mode.
                ; EI  ; CLI
                ; LDY #$7F        ; Mask for DSP data direction register.
                ; STY DSP         ; Set it up.
                ; LDA #$A7        ; KBD and DSP control register mask.
                ; STA KBDCR       ; Enable interrupts, set CA1, CB1, for
                ; STA DSPCR       ;  positive edge sense/output mode.
                ld      sp, $200
                ei
                ld      hl, IN_BUFF ; h = 2, l = 0
NOTCR:          
                call    getchar
                cp      $08             ; CMP #'_'+$80    ; "_"?
                jr      z, BACKSPACE    ; BEQ BACKSPACE   ; Yes.
                cp      $1B             ; CMP #$9B        ; ESC?
                jr      z, ESCAPE       ; BEQ ESCAPE      ; Yes.
                inc     l               ; INY             ; Advance text index.
                jp      p, NEXTCHAR     ; BPL NEXTCHAR    ; Auto ESC if > 127.
ESCAPE:         ; LDA #'\'+$80    ; "\".
                ; JSR ECHO              ; Output it.
GETLINE:        ld      a, $0d          ; LDA #$8D        ; CR.
                out     (CONIO), a      ; JSR ECHO        ; Output it.
                ld      a, $0a 
                out     (CONIO), a
                ld      l, 1            ; LDY #$01        ; Initialize text index.
BACKSPACE:      dec     l               ; DEY             ; Back up text index.
                jp      m, GETLINE      ; BMI GETLINE     ; Beyond start of line, reinitialize.
NEXTCHAR:       in      a, (CONSTA)     ; LDA KBDCR       ; Key ready?
                and     a
                jr      z, NEXTCHAR     ; BPL NEXTCHAR    ; Loop until ready.
                in      a, (CONIO)      ; LDA KBD         ; Load character. B7 should be ‘1’.
                ld      (hl), a         ; STA IN,Y        ; Add to text buffer.
                out     (CONIO), a      ; JSR ECHO        ; Display character.
                cp      $0d             ; CMP #$8D        ; CR?
                jr      nz, NOTCR       ; BNE NOTCR       ; No.
                ld      hl, $ff         ; LDY #$FF        ; Reset text index.
                ld      a, $00          ; LDA #$00        ; For XAM mode.
                id      ix, 0000h       ; TAX             ; 0->X.
SETSTOR:        ASL             ; Leaves $7B if setting STOR mode.
SETMODE:        STA MODE        ; $00=XAM, $7B=STOR, $AE=BLOCK XAM.
BLSKIP:         INY             ; Advance text index.
NEXTITEM:       LDA IN,Y        ; Get character.
                CMP #$8D        ; CR?
                BEQ GETLINE     ; Yes, done this line.
                CMP #'.'+$80    ; "."?
                BCC BLSKIP      ; Skip delimiter.
                BEQ SETMODE     ; Set BLOCK XAM mode.
                CMP #':'+$80    ; ":"?
                BEQ SETSTOR     ; Yes. Set STOR mode.
                CMP #'R'+$80    ; "R"?
                BEQ RUN         ; Yes. Run user program.
                STX L           ; $00->L.
                STX H           ;  and H.
                STY YSAV        ; Save Y for comparison.
NEXTHEX:        LDA IN,Y        ; Get character for hex test.
                EOR #$B0        ; Map digits to $0-9.
                CMP #$0A        ; Digit?
                BCC DIG         ; Yes.
                ADC #$88        ; Map letter "A"-"F" to $FA-FF.
                CMP #$FA        ; Hex letter?
                BCC NOTHEX      ; No, character not hex.
DIG:            ASL
                ASL             ; Hex digit to MSD of A.
                ASL
                ASL
                LDX #$04        ; Shift count.
HEXSHIFT:       ASL             ; Hex digit left, MSB to carry.
                ROL L           ; Rotate into LSD.
                ROL H           ; Rotate into MSD’s.
                DEX             ; Done 4 shifts?
                BNE HEXSHIFT    ; No, loop.
                INY             ; Advance text index.
                BNE NEXTHEX     ; Always taken. Check next character for hex.
NOTHEX:         CPY YSAV        ; Check if L, H empty (no hex digits).
                BEQ ESCAPE      ; Yes, generate ESC sequence.
                BIT MODE        ; Test MODE byte.
                BVC NOTSTOR     ; B6=0 STOR, 1 for XAM and BLOCK XAM
                LDA L           ; LSD’s of hex data.
                STA (STL,X)     ; Store at current ‘store index’.
                INC STL         ; Increment store index.
                BNE NEXTITEM    ; Get next item. (no carry).
                INC STH         ; Add carry to ‘store index’ high order.
TONEXTITEM:     JMP NEXTITEM    ; Get next command item.
RUN:            JMP (XAML)      ; Run at current XAM index.
NOTSTOR:        BMI XAMNEXT     ; B7=0 for XAM, 1 for BLOCK XAM.
                LDX #$02        ; Byte count.
SETADR:         LDA L-1,X       ; Copy hex data to
                STA STL-1,X     ;  ‘store index’.
                STA XAML-1,X    ; And to ‘XAM index’.
                DEX             ; Next of 2 bytes.
                BNE SETADR      ; Loop unless X=0.
NXTPRNT:        BNE PRDATA      ; NE means no address to print.
                LDA #$8D        ; CR.
                JSR ECHO        ; Output it.
                LDA XAMH        ; ‘Examine index’ high-order byte.
                JSR PRBYTE      ; Output it in hex format.
                LDA XAML        ; Low-order ‘examine index’ byte.
                JSR PRBYTE      ; Output it in hex format.
                LDA #':'+$80    ; ":".
                JSR ECHO        ; Output it.
PRDATA:         LDA #$A0        ; Blank.
                JSR ECHO        ; Output it.
                LDA (XAML,X)    ; Get data byte at ‘examine index’.
                JSR PRBYTE      ; Output it in hex format.
XAMNEXT:        STX MODE        ; 0->MODE (XAM mode).
                LDA XAML
                CMP L           ; Compare ‘examine index’ to hex data.
                LDA XAMH
                SBC H
                BCS TONEXTITEM  ; Not less, so no more data to output.
                INC XAML
                BNE MOD8CHK     ; Increment ‘examine index’.
                INC XAMH
MOD8CHK:        LDA XAML        ; Check low-order ‘examine index’ byte
                AND #$07        ;  For MOD 8=0
                BPL NXTPRNT     ; Always taken.
PRBYTE:         PHA             ; Save A for LSD.
                LSR
                LSR
                LSR             ; MSD to LSD position.
                LSR
                JSR PRHEX       ; Output hex digit.
                PLA             ; Restore A.
PRHEX:          AND #$0F        ; Mask LSD for hex print.
                ORA #'0'+$80    ; Add "0".
                CMP #$BA        ; Digit?
                BCC ECHO        ; Yes, output it.
                ADC #$06        ; Add offset for letter.

ECHO:           BIT DSP         ; DA bit (B7) cleared yet?
                BMI ECHO        ; No, wait for display.
                STA DSP         ; Output character. Sets DA.
                RTS             ; Return.

                BRK             ; unused
                BRK             ; unused

; Interrupt Vectors

                .WORD $0F00     ; NMI
                .WORD RESET     ; RESET
                .WORD $0000     ; BRK/IRQ
