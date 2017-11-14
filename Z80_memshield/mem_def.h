#ifndef _MEM_DEF_H_
#define _MEM_DEF_H_

#include <AVR/io.h>

#define SRAM_ADDRL_PORT   PORTL
#define SRAM_ADDRH_PORT   PORTC
#define SRAM_ADDRL_MASK   0xff
#define SRAM_ADDRH_MASK   0xff

#define SRAM_ADDRX0_PORT  PORTJ
#define SRAM_ADDRX0       PJ0
#define SRAM_ADDRX0_PIN   14
#define SRAM_ADDRX1_PORT  PORTJ
#define SRAM_ADDRX1       PJ1
#define SRAM_ADDRX1_PIN   15
#define SRAM_ADDRX2_PORT  PORTH
#define SRAM_ADDRX2       PH1
#define SRAM_ADDRX2_PIN   16
#define SRAM_ADDRX0_MASK  (1<<0)
#define SRAM_ADDRX1_MASK  (1<<1)
#define SRAM_ADDRX2_MASK  (1<<2)
#define SRAM_ADDRX_MASK   ( SRAM_ADDRX0_MASK | SRAM_ADDRX1_MASK | SRAM_ADDRX2_MASK )

#define SRAM_DATA_OUT     PORTA
#define SRAM_DATA_IN      PINA
#define SRAM_DATA_DDR     DDRA
#define SRAM_DATA_MASK    0xff

// CS, RD, WR, ALE, ALE_OE
#define SRAM_CS_PIN     38
#define SRAM_CS_PORT    PORTD
#define SRAM_CS         PD7
#define SRAM_ALE_PIN    39
#define SRAM_ALE_PORT   PORTG
#define SRAM_ALE        PG2
#define SRAM_ALE_OE_PIN  17
#define SRAM_ALE_OE_PORT PORTH
#define SRAM_ALE_OE      PH0
#define SRAM_OE_PIN     40
#define SRAM_OE_PORT    PORTG
#define SRAM_OE         PG1
#define SRAM_WE_PIN     41
#define SRAM_WE_PORT    PORTG
#define SRAM_WE         PG0

#endif /* _MEM_DEF_H_ */

