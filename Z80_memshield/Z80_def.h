#ifndef _Z80_DEF_H_
#define _Z80_DEF_H_

#include <Arduino.h>

#define Z80_CLK_PIN     11 // OC1A

#define Z80_DATA_BUS    PORTA
#define Z80_ADDRL_BUS   PORTL
#define Z80_ADDRH_BUS   PORTC

#define Z80_MREQ_PIN    38   // CS (PD7)
#define Z80_IORQ_PIN    5
#define Z80_RD_PIN      40   // RD
#define Z80_WR_PIN      41   // WR

#define Z80_INT_PIN     13
#define Z80_NMI_PIN     12
#define Z80_HALT_PIN    10

#define Z80_BUSACK_PIN  6
#define Z80_WAIT_PIN    7
#define Z80_BUSREQ_PIN  8
#define Z80_RESET_PIN   9

#define Z80_M1_PIN 		3
#undef Z80_RFSH_PIN

#endif /* _Z80_DEF_H_ */

