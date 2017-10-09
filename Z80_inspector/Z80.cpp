#include "Z80.h"

void Z80_bus_init(void) {
  Z80_ADDRL_DDR = 0x00;
  Z80_ADDRH_DDR = 0x00;
  Z80_DATA_DDR = 0x00;

  pinMode(Z80_MREQ_PIN, INPUT);
  pinMode(Z80_IORQ_PIN, INPUT);
  pinMode(Z80_RD_PIN, INPUT);
  pinMode(Z80_WR_PIN, INPUT);
}

void Z80_status_init() {
  pinMode(Z80_CLK_PIN, OUTPUT);
  
  pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, HIGH);
  pinMode(Z80_NMI_PIN, OUTPUT);
  digitalWrite(Z80_NMI_PIN, HIGH);
  pinMode(Z80_INT_PIN, OUTPUT);
  digitalWrite(Z80_INT_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
}

void Z80_reset(bool v) { digitalWrite(Z80_RESET_PIN, (v != LOW) ); }
void Z80_busreq(bool v) { digitalWrite(Z80_BUSREQ_PIN, (v != LOW) ); }
uint8_t Z80_busack() {  return digitalRead(Z80_BUSACK_PIN); }
uint8_t Z80_halt() { return digitalRead(Z80_HALT_PIN); }

uint8_t Z80_mreq() { return digitalRead(Z80_MREQ_PIN); }
uint8_t Z80_iorq() { return digitalRead(Z80_IORQ_PIN); }
uint8_t Z80_read() { return digitalRead(Z80_RD_PIN); }
uint8_t Z80_write() { return digitalRead(Z80_WR_PIN); }
uint8_t Z80_M1() { return digitalRead(Z80_M1_PIN); }

uint8_t Z80_busmode() {
  return ( (!Z80_iorq())<<3 | (!Z80_mreq())<<2 | (!Z80_write())<<1 | (!Z80_read())<<0 );
}

uint16_t Z80_addressbus() { return ((((uint16_t)PINC)<<8) | PINA); }

void Z80_databus_mode(uint8_t inout) {
  if ( inout == OUTPUT )
    Z80_DATA_DIR = 0xff;
  else 
    Z80_DATA_DIR = 0x00;
}

uint8_t Z80_databus() {
  Z80_DATA_DIR = 0x00; 
  return Z80_DATA_IN; 
}

uint8_t Z80_databus_write(uint8_t data) { 
  Z80_DATA_DIR = 0xff;
  Z80_DATA_OUT = data;
  return data; 
}

