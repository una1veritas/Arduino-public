;    CP/M IPL for avrcpm
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

CON$IN		EQU	81H
CON$OUT		EQU	82H
TRACK$SEL$L 	EQU	0A0H
TRACK$SEL$H	EQU	0A1H
SECTOR$SEL	EQU	0A2H
ADR$L		EQU	0A4H
ADR$H		EQU	0A5H
EXEC$DMA	EQU	0A6H

SEC$PER$TR	EQU	64		;sectors per track
SYS$SECT	EQU	55		;load system sector count

MSIZE		EQU	62		;size of available RAM in k

BIAS		EQU	(MSIZE-20) * 1024 
CCP		EQU	3400H+BIAS	;base of cpm ccp
BIOS		EQU	CCP+1600H	;base of bios

CR		EQU	13
LF		EQU	10

	ORG	2000H
	; IPL for the CP/M-emu in an AVR. Loads CPM from the 'disk' from
	; track 0 sector 2 to track 1 sector 26.

	LXI SP,1000H

	LXI H,MSGIPL
	CALL PRINTIPL

	MVI B,SYS$SECT
	LXI D,0001H
	LXI H,CCP
LOADLOOP:
	MOV A,D		;track
	OUT TRACK$SEL$L
	OUT TRACK$SEL$H
	MOV A,E 	;sector (base-0)
	OUT SECTOR$SEL
	MOV A,L 	;dma L
	OUT ADR$L
	MOV A,H 	;dma H
	OUT ADR$H
	MVI A,1
	OUT EXEC$DMA

	PUSH B
	LXI B,80H
	DAD B
	POP B

	INR E
	MOV A,E
	CPI SEC$PER$TR
	JNZ NONEXTTRACK

	INR D
	MVI E,0

NONEXTTRACK:

	DCR B
	JNZ LOADLOOP

	JMP BIOS

PRINTIPL:
	MOV	A,M
	ORA	A
	RZ
	OUT	CON$OUT
	INX	H
	JMP	PRINTIPL

MSGIPL:
	DB	CR,LF
	DB	'IPL is loading system...'
	DB	0

END
