#ifndef _Z80_PINDEF_H_
#define _Z80_PINDEF_H_

//PB5 (OC1A) 
#define Z80_CLK_PIN     11 // PB5 OC1A

#define Z80_ADDRL_PORT  PORTK
#define Z80_ADDRH_PORT  PORTL
#define Z80_DATA_OUT    PORTF
#define Z80_DATA_IN     PINF

#define Z80_MREQ_PIN    2 
#define Z80_IORQ_PIN    3

#define Z80_RD_PIN      4
#define Z80_WR_PIN      5

#define Z80_BUSACK_PIN  6
#define Z80_WAIT_PIN    7
#define Z80_BUSREQ_PIN  16
#define Z80_RESET_PIN   15
#define Z80_M1_PIN      14
#define Z80_RFSH_PIN    12

#define Z80_INT_PIN     8
#define Z80_NMI_PIN     9
#define Z80_HALT_PIN    10

#endif /*_Z80_PINDEF_H_ */

