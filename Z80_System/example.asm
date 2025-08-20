START:
        LD   HL, $0030
        LD   IX, $0024
        LD   (IX+0), 100+1
        LD   A, 1
LOOP:
        CP   (HL)
        JR   Z, CARRY
        LD   (HL), 1
        DEC  (IX+0)
        JR   Z, DONE
        JR   REWIND
CARRY:
        INC  HL
        JR   LOOP
REWIND:
        DEC  HL
        CP   (HL)
        JR   NZ, RIGHT
        LD  (HL), 0
        JR   REWIND
RIGHT:
        INC  HL
        JR LOOP

DONE:
        HALT               
