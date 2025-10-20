;    CP/M BIOS for avrcpm
;    Copyright (C) 2010 Sprite$tm
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
;
;-------------------------------------------------------------------------
;    CONVERT FOR MAC.COM ASSEMBLER  by neko Java 2014.5.3

CON$STS$P	EQU	80H
CON$IN$P	EQU	81H
CON$OUT$P	EQU	82H
TRACK$SEL$L 	EQU	0A0H
TRACK$SEL$H	EQU	0A1H
SECTOR$SEL	EQU	0A2H
ADR$L		EQU	0A4H
ADR$H		EQU	0A5H
EXEC$DMA	EQU	0A6H
DMA$RS		EQU	0A7H

NDISK		EQU	4	;number of drives.
SEC$PER$TRK	EQU	64	;sectors/track

MSIZE	EQU	62	;size of available RAM in k

BIAS	EQU	(MSIZE-20) * 1024 
CCP	EQU	3400H+BIAS	;base of cpm ccp
BDOS	EQU	CCP+806H	;base of bdos
BIOS	EQU	CCP+1600H	;base of bios
CDISK	EQU	0004H		;current disk number (0 ... 15)
IOBYTE	EQU	0003H		;intel iobyte
BUFF	EQU	0080H		;default buffer address
RETRY	EQU	3		;max retries on disk i/o before error

CR	EQU	13
LF	EQU	10

	ORG	BIOS
NSECTS	EQU	($-CCP)/128	;warm start sector count
	
	JMP BOOT
WBOOTE:	
	JMP WBOOT
	JMP CONST
	JMP CONIN
	JMP CONOUT
	JMP LIST
	JMP PUNCH
	JMP READER
	JMP HOME
	JMP SELDSK
	JMP SETTRK
	JMP SETSEC
	JMP SETDMA
	JMP READ
	JMP WRITE
	JMP LISTST
	JMP SECTRAN

SIGNON:
	DB	CR,LF
	DB	MSIZE/10+'0'
	DB	MSIZE - (MSIZE/10)*10 + '0'	;modulo doesn't work?
	DB	'k CP/M Version 2.2 COPYRIGHT (C) 1979, DIGITAL RESEARCH'
	DB	CR,LF,0

BOOT:
	LXI	SP,BUFF
	LXI	H,SIGNON
	CALL	PRMSG
	XRA	A
	STA	IOBYTE
	STA	CDISK
	STA	ODISK		;add neko Java 2011.7.27
	JMP	GOCPM 

WBOOT:	;re-load CP/M
	LXI	SP,BUFF
	MVI	C,0
	CALL	SELDSK
	CALL	HOME
	MVI	B,NSECTS
	MVI	C,0		;track
	MVI	D,1		;sektor (0 based)
	LXI	H,CCP
LOAD1:
	PUSH	B
	PUSH	D
	PUSH	H
	MOV	C,D
	CALL	SETSEC
	POP	B
	PUSH	B
	CALL	SETDMA
	CALL	READ
	CPI	0		;read error?
	JNZ	WBOOT
	
	POP	H
	LXI	D,128
	DAD	D
	POP	D
	POP	B
	DCR	B
	JZ	GOCPM
	
	INR	D
	MOV	A,D
	CPI	SEC$PER$TRK	;if sector >= 64 then change tracks. mod neko Java 2011.7.19
	JC	LOAD1
	
	MVI	D,0
	INR	C
	PUSH	B
	PUSH	D
	PUSH	H
	MVI	B,0	;add neko Java 2011.7.19
	CALL	SETTRK
	POP	H
	POP	D
	POP	B
	JMP	LOAD1
	
GOCPM:
	MVI	A,0C3H
	STA	0
	LXI	H,WBOOTE
	SHLD	1
	STA	5
	LXI	H,BDOS
	SHLD	6
		
	LXI	B,BUFF
	CALL	SETDMA
	LDA	CDISK
	MOV	C,A
	JMP	CCP

CONST:
	IN	CON$STS$P
	RET

CONIN:
	IN	CON$STS$P
	CPI	0FFH
	JNZ	CONIN

	IN	CON$IN$P
	RET

CONOUT:
	MOV	A,C
	OUT	CON$OUT$P
	RET

LIST:
	RET

LISTST:
	MVI	A,0
	RET

PUNCH:
	RET

READER:
	MVI	A,1FH
	RET

HOME:
	PUSH	PSW
	MVI	A,0
	OUT	TRACK$SEL$L
	OUT	TRACK$SEL$H	;add neko Java 2011.7.19
	POP	PSW
	RET

SELDSK:			;mod for multi-drives. neko Java 2014.5.4
	PUSH	PSW
	MOV	A,C
	CPI	NDISK
	JNC	SELDSK$NA
	MOV	L,A
	MVI	H,0
	DAD	H
	DAD	H
	DAD	H
	DAD	H
	PUSH	D
	LXI	D,DPE0		;disk parameter header base address
	DAD	D
	POP	D
	STA	ODISK
	POP	PSW
	RET
SELDSK$NA:
	LXI	H,0
	LDA	ODISK
	STA	CDISK
	POP	PSW
	RET

SETTRK:
	PUSH	PSW
	MOV	A,C
	OUT	TRACK$SEL$L
	MOV	A,B			;add neko Java 2011.7.19
	OUT	TRACK$SEL$H		;add neko Java 2011.7.19
	POP	PSW
	RET

SETSEC:
	PUSH	PSW
	MOV	A,C
	OUT	SECTOR$SEL
	POP	PSW
	RET

SETDMA:
	PUSH	PSW
	MOV	A,C
	OUT	ADR$L
	MOV	A,B
	OUT	ADR$H
	POP	PSW
	RET

READ:
	MVI	A,1
	OUT	EXEC$DMA
	IN	DMA$RS	;mod neko Java
	RET

WRITE:
	MVI	A,2
	OUT	EXEC$DMA
	IN	DMA$RS	;mod neko Java
	RET

SECTRAN:
	;translate sector bc using table at de, res into hl
	;not implemented :)
	MOV	H,B
	MOV	L,C
	RET

PRMSG:
	MOV	A,M
	ORA	A
	RZ
	PUSH	H
	MOV	C,A
	CALL	CONOUT
	POP	H
	INX	H
	JMP	PRMSG

ODISK:
	DS	1	;add neko Java 2011.7.27

;DISKDEF MACRO ----------------------------------------------------------------------
;
;DISKDEF	dn, fsc, lsc, skf, bls, dks, dir, cks, ofs
;
;dn	:logical disk number. 0=A, 1=B,..., 15=P
;fsc	:first sector number. 0 or 1
;lsc	:last sector number.
;skf	:optional 'skew factor' for sector translate.
;bls	:data block size.(byte)
;dks	:disk capacity except the system track.(bls)
;dir	:the number of directory entries.
;cks	:same as 'dir' in the case of an exchangeable device. 0 in the case of others.
;ofs	:the number of tracks to skip.
;
;    [128byte/sector]*[64sector/track]*[244tracks]*4drives(partition)
;    (ofs=1, bls=2048, dir=256)
;    (A drive: dks=128*64*243/2048=972, ofs=1)
;    (B drive: dks=128*64*244/2048=976, ofs=244)
;    (C drive: dks=128*64*244/2048=976, ofs=244+244=488)
;    (D drive: dks=128*64*244/2048=976, ofs=244+244+244=732)
;

MACLIB	DISKDEF
ORG	$	;macro start address

DISKS	NDISK	;number of drives

DISKDEF	0,1,64,,2048,972,256,0,1
DISKDEF	1,1,64,,2048,976,256,0,244
DISKDEF	2,1,64,,2048,976,256,0,488
DISKDEF	3,1,64,,2048,976,256,0,732

ORG	$	;scratch area address

ENDEF

END
