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
