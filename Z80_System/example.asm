START:
        LD   HL, VAR
        LD   SP, 007eh
        XOR  A
LOOP:
		LD   (HL), A
		INC  A
		CP   100
		JR   Z, PRINT
		CALL MEMDUMP
		JR   LOOP

MEMDUMP:
        OUT  ($ff), A
        RET

PRINT:
        LD	HL, MSG
PRINT_LOOP:
        LD	A, (HL)
        OR 	0
        JR 	Z, DONE
        OUT (2), A
        INC HL
        JR 	PRINT_LOOP
DONE:
		HALT
MSG:
		DB	"Hello, my friends!"
		DB	13, 10
		DB 	0
VAR:
		DB 	0
