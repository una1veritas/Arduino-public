
; MONITOR EQUATES (RESTART INSTRUCTIONS)

_ROUT   EQU     0F7H            ; ROUT - Output char in A
_BLNK   EQU     07BDFH          ; SCAL BLINK - Get input char in A
_INLN   EQU     063DFH          ; SCAL INLIN - Get input line
_MFLP   EQU     05FDFH          ; SCAL MFLP - Toggle tape drv LED
_MRET   EQU     05BDFH          ; SCAL MRET - Return to monitor
_READ   EQU     052DFH          ; SCAL READ
_RIN    EQU     062DFH          ; SCAL RIN - Scan for input char
_VRFY   EQU     056DFH          ; SCAL VERIFY
_WRIT   EQU     057DFH          ; SCAL WRITE

; GENERAL EQUATES

UARTD   EQU     01H             ; UART data port
UARTS   EQU     02H             ; UART status port
CTRLC   EQU     03H             ; Control "C"
CTRLG   EQU     07H             ; Control "G"
BKSP    EQU     08H             ; Back space
LF      EQU     0AH             ; Line feed
CS      EQU     0CH             ; Clear screen
CR      EQU     0DH             ; Carriage return
CTRLO   EQU     0FH             ; Control "O"
CTRLR   EQU     12H             ; Control "R"
CTRLS   EQU     13H             ; Control "S"
CTRLU   EQU     15H             ; Control "U"
CTRLZ   EQU     1AH             ; Control "Z"
ESC     EQU     1BH             ; Escape
TBRK    EQU     1CH             ; "T" monitor break
TBS     EQU     1DH             ; "T" monitor back space
TCS     EQU     1EH             ; "T" monitor clear screen
TCR     EQU     1FH             ; "T" monitor carriage return
DEL     EQU     7FH             ; Delete

; MONITOR LOCATIONS

MONSTT  EQU     0000H           ; Start of monitor
STMON   EQU     000DH           ; NAS-SYS initialisation
MFLP    EQU     0051H           ; Flip tape LED ("T")
MONTYP  EQU     008DH           ; Type of "T" monitor
T2DUMP  EQU     03D1H           ; "T2" Dump routine
T4WR    EQU     0400H           ; "T4" Write routine
T4READ  EQU     070CH           ; "T4" Read routine
VDU     EQU     0800H           ; NASCOM Video RAM base

; MONITOR WORK SPACE LOCATIONS

PORT0   EQU     0C00H           ; Copy of output port 0
ARG1    EQU     0C0CH           ; Argument 1
ARG2    EQU     0C0EH           ; Argument 2
TCUR    EQU     0C18H           ; "T" monitor cursor
CURSOR  EQU     0C29H           ; NAS-SYS Cursor
ARGN    EQU     0C2BH           ; Number of ARGS
TOUT    EQU     0C4AH           ; "T" Output reflection
TIN     EQU     0C4DH           ; "T" Input reflection
CIN     EQU     0C75H           ; NAS-SYS Input table
NMI     EQU     0C7EH           ; NAS-SYS NMI Jump

; BASIC WORK SPACE LOCATIONS

WRKSPC  EQU     1000H           ; BASIC Work space
USR     EQU     1003H           ; "USR (x)" jump
OUTSUB  EQU     1006H           ; "OUT p,n"
OTPORT  EQU     1007H           ; Port (p)
DIVSUP  EQU     1009H           ; Division support routine
DIV1    EQU     100AH           ; <- Values
DIV2    EQU     100EH           ; <-   to
DIV3    EQU     1012H           ; <-   be
DIV4    EQU     1015H           ; <-inserted
SEED    EQU     1017H           ; Random number seed
LSTRND  EQU     103AH           ; Last random number
INPSUB  EQU     103EH           ; #INP (x)" Routine
INPORT  EQU     103FH           ; PORT (x)
NULLS   EQU     1041H           ; Number of nulls
LWIDTH  EQU     1042H           ; Terminal width
COMMAN  EQU     1043H           ; Width for commas
NULFLG  EQU     1044H           ; Null after input byte flag
CTLOFG  EQU     1045H           ; Control "O" flag
LINESC  EQU     1046H           ; Lines counter
LINESN  EQU     1048H           ; Lines number
CHKSUM  EQU     104AH           ; Array load/save check sum
NMIFLG  EQU     104CH           ; Flag for NMI break routine
BRKFLG  EQU     104DH           ; Break flag
RINPUT  EQU     104EH           ; Input reflection
POINT   EQU     1051H           ; "POINT" reflection (unused)
PSET    EQU     1054H           ; "SET"   reflection
RESET   EQU     1057H           ; "RESET" reflection
STRSPC  EQU     105AH           ; Bottom of string space
LINEAT  EQU     105CH           ; Current line number
BASTXT  EQU     105EH           ; Pointer to start of program
BUFFER  EQU     1061H           ; Input buffer
STACK   EQU     1066H           ; Initial stack
CURPOS  EQU     10ABH           ; Character position on line
LCRFLG  EQU     10ACH           ; Locate/Create flag
TYPE    EQU     10ADH           ; Data type flag
DATFLG  EQU     10AEH           ; Literal statement flag
LSTRAM  EQU     10AFH           ; Last available RAM
TMSTPT  EQU     10B1H           ; Temporary string pointer
TMSTPL  EQU     10B3H           ; Temporary string pool
TMPSTR  EQU     10BFH           ; Temporary string
STRBOT  EQU     10C3H           ; Bottom of string space
CUROPR  EQU     10C5H           ; Current operator in EVAL
LOOPST  EQU     10C7H           ; First statement of loop
DATLIN  EQU     10C9H           ; Line of current DATA item
FORFLG  EQU     10CBH           ; "FOR" loop flag
LSTBIN  EQU     10CCH           ; Last byte entered
READFG  EQU     10CDH           ; Read/Input flag
BRKLIN  EQU     10CEH           ; Line of break
NXTOPR  EQU     10D0H           ; Next operator in EVAL
ERRLIN  EQU     10D2H           ; Line of error
CONTAD  EQU     10D4H           ; Where to CONTinue
PROGND  EQU     10D6H           ; End of program
VAREND  EQU     10D8H           ; End of variables
ARREND  EQU     10DAH           ; End of arrays
NXTDAT  EQU     10DCH           ; Next data item
FNRGNM  EQU     10DEH           ; Name of FN argument
FNARG   EQU     10E0H           ; FN argument value
FPREG   EQU     10E4H           ; Floating point register
FPEXP   EQU     FPREG+3         ; Floating point exponent
SGNRES  EQU     10E8H           ; Sign of result
PBUFF   EQU     10E9H           ; Number print buffer
MULVAL  EQU     10F6H           ; Multiplier
PROGST  EQU     10F9H           ; Start of program text area
STLOOK  EQU     115DH           ; Start of memory test

; BASIC ERROR CODE VALUES

NF      EQU     00H             ; NEXT without FOR
SN      EQU     02H             ; Syntax error
RG      EQU     04H             ; RETURN without GOSUB
OD      EQU     06H             ; Out of DATA
FC      EQU     08H             ; Function call error
OV      EQU     0AH             ; Overflow
OM      EQU     0CH             ; Out of memory
UL      EQU     0EH             ; Undefined line number
BS      EQU     10H             ; Bad subscript
DD      EQU     12H             ; Re-DIMensioned array
DZ      EQU     14H             ; Division by zero (/0)
ID      EQU     16H             ; Illegal direct
TM      EQU     18H             ; Type miss-match
OS      EQU     1AH             ; Out of string space
LS      EQU     1CH             ; String too long
ST      EQU     1EH             ; String formula too complex
CN      EQU     20H             ; Can't CONTinue
UF      EQU     22H             ; UnDEFined FN function
MO      EQU     24H             ; Missing operand
