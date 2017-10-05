#ifndef _SRAM_PINDEF_H_
#define _SRAM_PINDEF_H_

#include <AVR/io.h>

#define SRAM_ADDRL_PORT   PORTA
#define SRAM_ADDRH_PORT   PORTC
#define SRAM_ADDRL_MASK   0xff
#define SRAM_ADDRH_MASK   0xff
#define SRAM_DATA_PORT    PORTF
#define SRAM_DATA_MASK    0xff

// PH0
#define SRAM_BANK_PORT    PORTH
#define SRAM_BANK_MASK    0x01
#define SRAM_BANK_PIN     17


// PG5
#define SRAM_CS_PIN       4
// RD, WR, ALE
//#define SRAM_ALE_PIN    39
#define SRAM_OE_PIN       40
#define SRAM_WE_PIN       41
// PH6
#define SRAM_E2_PIN       9

#endif /* _SRAM_PINDEF_H_ */


