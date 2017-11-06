
#include "Z80.h"
#include "common.h"

#include <util/delay.h>

#undef Z80_WAIT_INTERNAL_PULLUP

void z80_bus_setup(void) {
  // set data & address bus mode to input, and clear pull-ups
  DDR(Z80_DATA_BUS) = 0;
  Z80_DATA_BUS = 0;
  DDR(Z80_ADDRL_BUS) = 0;
  Z80_ADDRL_BUS = 0;
  DDR(Z80_ADDRH_BUS) = 0;
  Z80_ADDRH_BUS = 0;

  pinMode(Z80_CLK_PIN, OUTPUT);
  
  // set pin mode to INPUT for reading the outputs of Z80
  pinMode(Z80_MREQ_PIN, INPUT);
  pinMode(Z80_IORQ_PIN, INPUT);
  pinMode(Z80_RD_PIN, INPUT);
  pinMode(Z80_WR_PIN, INPUT);
  pinMode(Z80_BUSACK_PIN, INPUT);
#ifdef Z80_M1_PIN
  pinMode(Z80_M1_PIN, INPUT);
#endif
#ifdef Z80_RFSH_PIN
  pinMode(Z80_RFSH_PIN, INPUT);
#endif

  // Z80 input pins
  // set pin mode default to INPUT and set atmega pull-ups if necessary
#ifdef Z80_NMI_PIN
  pinMode(Z80_NMI_PIN, INPUT);	  //digitalWrite(Z80_NMI_PIN, HIGH);
#endif
#ifdef Z80_INT_PIN
  pinMode(Z80_INT_PIN, INPUT);	  //digitalWrite(Z80_INT_PIN);
#endif
  pinMode(Z80_BUSREQ_PIN, INPUT);  	// digitalWrite(Z80_BUSREQ_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, INPUT);
#ifdef Z80_WAIT_INTERNAL_PULLUP
  digitalWrite(Z80_WAIT_PIN, HIGH);
#endif
  pinMode(Z80_RESET_PIN, INPUT);  // digitalWrite(Z80_RESET_PIN, HIGH);

}

void z80_reset(unsigned long dur) {
	pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(dur);
  digitalWrite(Z80_RESET_PIN, HIGH);
	pinMode(Z80_RESET_PIN, INPUT);
}

void z80_busreq(BYTE val) {
	if ( val ) {
		pinMode(Z80_BUSREQ_PIN, INPUT);
		digitalWrite(Z80_BUSREQ_PIN, HIGH);
	} else {
		pinMode(Z80_BUSREQ_PIN, OUTPUT);
		digitalWrite(Z80_BUSREQ_PIN, LOW);
	}
}

BYTE z80_busack(WORD wcnt) {
	while ( wcnt > 0 && digitalRead(Z80_BUSACK_PIN) ) {
		_delay_us(4);
	}
	return digitalRead(Z80_BUSACK_PIN);
}

BYTE z80_mreq_rd() {
	return digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_RD_PIN);
}

BYTE z80_mreq_wr() {
	return digitalRead(Z80_MREQ_PIN) || digitalRead(Z80_WR_PIN);
}

BYTE z80_iorq_rd() {
	return digitalRead(Z80_IORQ_PIN) || digitalRead(Z80_RD_PIN);
}

BYTE z80_iorq_wr(){
	return digitalRead(Z80_IORQ_PIN) || digitalRead(Z80_WR_PIN);
}


void z80_wait_enable() {
	pinMode(Z80_WAIT_PIN, INPUT);
#ifdef Z80_WAIT_INTERNAL_PULLUP
	digitalWrite(Z80_WAIT_PIN, HIGH);
#endif
}

void z80_wait_disable() {
	pinMode(Z80_WAIT_PIN, OUTPUT);
	digitalWrite(Z80_WAIT_PIN, HIGH);
}

