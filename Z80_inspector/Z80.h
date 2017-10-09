#ifndef _Z80_H_
#define _Z80_H_

#include <Arduino.h>

/*
 * Clock Source
 * OC1A PB5
 */
#define Z80_CLK_PIN     11 

/*
 * Address & Data Bus
 * Address Low 8  PORTA
 * Address High 8 PORTC
 * Data PORTF
 * RD PG5 (RD) 40
 * WR PG0 (WR) 41
 * MREQ PG2 (ALE) 39
 * IORQ PD7       38
 */
#define Z80_ADDRL_DDR   DDRA
#define Z80_ADDRH_DDR   DDRC
#define Z80_DATA_DDR    DDRF
#define Z80_ADDRL_PORT  PORTA
#define Z80_ADDRH_PORT  PORTC
#define Z80_DATA_DIR    DDRF
#define Z80_DATA_OUT    PORTF
#define Z80_DATA_IN     PINF

#define Z80_IORQ_PIN    38
#define Z80_MREQ_PIN    39
#define Z80_RD_PIN      40
#define Z80_WR_PIN      41

/*
 * Bus & Status Request & Response Pins
 */
#define Z80_INT_PIN     3
#define Z80_NMI_PIN     4
#define Z80_HALT_PIN    5

#define Z80_BUSACK_PIN  6
#define Z80_WAIT_PIN    7
#define Z80_BUSREQ_PIN  8
#define Z80_RESET_PIN   9
#define Z80_M1_PIN      10
#define Z80_RFSH_PIN    12


void Z80_bus_init(void);
void Z80_status_init();

void Z80_reset(bool v);
void Z80_busreq(bool v);
uint8_t Z80_busack();
uint8_t Z80_halt();

uint8_t Z80_mreq();
uint8_t Z80_iorq();
uint8_t Z80_read();
uint8_t Z80_write();
uint8_t Z80_M1();
uint8_t Z80_busmode();

void Z80_databus_mode(uint8_t inout);
uint16_t Z80_addressbus();
uint8_t Z80_databus();
uint8_t Z80_databus_write(uint8_t);

#endif /*_Z80_H_ */

