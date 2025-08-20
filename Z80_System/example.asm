START:
        LD   HL, 0030h
        LD   SP, 0040h
        XOR  A
LOOP:
		LD   (HL), A
		INC  A
		CP   100
		JR   Z, DONE
		CALL MEMDUMP
		JR   LOOP

MEMDUMP:
        OUT  ($ff), A
        RET

DONE:
        HALT
