;
SBOTTOM equ 	00f80h
MONITOR equ     01000h
;
	    org 	0000h
RST_00: 	; Resetr/Power on
	    ld 		sp, SBOTTOM
	    jp  	MONITOR
;
;		org 	0038h
;RST_38:		; Mode 1 interrupt service routine
;
;		org 	0066h
;RST_66: 	; NMI interrupt service routine

;
MSIZE	EQU	64		;mem size in kbytes
;
BIAS	EQU	(MSIZE-20)*1024	;offset from 20k system
CCP	EQU	3400H+BIAS	;base of the ccp
BIOS	EQU	CCP+1600H	;base of the bios
BIOSL	EQU	0300H		;length of the bios
BOOT	EQU	BIOS
SIZE	EQU	BIOS+BIOSL-CCP	;size of cp/m system
SECTS	EQU	SIZE/128	;# of sectors to load
;
;	I/O ports
;
CONDAT	EQU	1		;console data port
DRIVE   EQU	10		;fdc-port: # of drive
TRACK   EQU	11		;fdc-port: # of track
SECTOR  EQU	12		;fdc-port: # of sector
FDCOP   EQU	13		;fdc-port: command
FDCST   EQU	14		;fdc-port: status
DMAL    EQU	15		;dma-port: dma address low
DMAH    EQU	16		;dma-port: dma address high
;
        org     00040h
        ld  bc, 0002h
        ld  hl, 0800h
LSECT:	LD	A,B		;set track
	    OUT	(TRACK),A
	    LD	A,C		;set sector
        OUT	(SECTOR),A
        LD	A,L		;set dma address low
        OUT	(DMAL),A
        LD	A,H		;set dma address high
        OUT	(DMAH),A
        XOR	A		;read sector
        OUT	(FDCOP),A
        IN	A,(FDCST)	;get status of fdc
        OR	A		;read successful ?
        JP	Z, MONITOR		;yes, continue
        LD	HL,ERRMSG	;no, print error
PRTMSG:	LD	A,(HL)
        OR	A
        JP	Z,STOP
        OUT	(CONDAT),A
        INC	HL
        JP	PRTMSG
STOP:	JP  MONITOR

ERRMSG:	DEFM	'BOOT: error booting'
	    DEFB	13,10,0