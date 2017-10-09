#ifndef _SRAM_PINDEF_H_
#define _SRAM_PINDEF_H_

#include <AVR/io.h>

#define SRAM_ADDRL_PORT   PORTA
#define SRAM_ADDRH_PORT   PORTC
#define SRAM_ADDRL_MASK   0xff
#define SRAM_ADDRH_MASK   0xff
#define SRAM_ADDRX_PORT   PORTH
#define SRAM_ADDRX_MASK   0x01
#define SRAM_ADDRX_PIN    17

#define SRAM_DATA_OUT     PORTA
#define SRAM_DATA_IN      PINA
#define SRAM_DATA_DDR     DDRA
#define SRAM_DATA_MASK    0xff

// PD7
#define SRAM_CS_PIN       38

// RD, WR, ALE
#define SRAM_ALE_PIN    39
#define SRAM_OE_PIN       40
#define SRAM_WE_PIN       41

//VCC
//#define SRAM_E2_PIN       9

#endif /* _SRAM_PINDEF_H_ */


