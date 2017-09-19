#include <avr/io.h>

// d11 (OC1A)   --> Z80 CLK
#define Z80_CLK   (1<<PB5)
#define Z80_CLK_PIN 11
// d12      --> Z80 ~RESET
#define Z80_RESET   (1<<PB6)
#define Z80_RESET_PIN 12

// d5         --> Z80 ~INT
#define Z80_INT_PIN 5
#define Z80_INT   (1<<PE3)
// d6       --> Z80 ~NMI
#define Z80_NMI_PIN 6
#define Z80_NMI   (1<<PH3)

// d7       --> Z80 ~WAIT
#define Z80_WAIT (1<<PH4)
#define Z80_WAIT_PIN 7
// d8       --> Z80 ~BUSREQ
#define Z80_BUSREQ  (1<<PH5)
#define Z80_BUSREQ_PIN 8
/*
#define Z80_HALT_PIN  38
#define Z80_HALT  (1<<PD7)
// d39 PG2      --> Z80 ~IOREQ (OUT)
#define Z80_IOREQ_PIN 39
#define Z80_IOREQ   (1<<PG2)
// d45 PL4      --> Z80 ~MREQ (OUT)
#define Z80_MREQ_PIN 45
#define Z80_MREQ  (1<<PL4)
*/
// d40 PG1      --> SRAM ~OE (IN), Z80 ~RD (OUT)
// d41 PG0      --> SRAM ~WE (IN), Z80 ~WR (OUT)
// d4  PG5      --> SRAM ~CS (IN), Z80 ~MREQ (OUT)
/*
#define Z80_RD_PIN  40
#define Z80_RD    (1<<PG1)
#define Z80_WR_PIN  41
#define Z80_WR    (1<<PG0)
#define Z80_MREQ_PIN 4
#define Z80_MREQ (1<<PG5)
// d8 PH5     --> Z80 ~BUSACK (OUT)
#define Z80_BUSACK_PIN 8
#define Z80_BUSACK (1<<PH5)
*/

