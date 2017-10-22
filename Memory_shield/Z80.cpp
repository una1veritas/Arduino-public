#include "Z80.h"
#include "common.h"

void z80_bus_init(void) {
  // set data & address bus mode to input, and clear pull-ups
  DDR(Z80_DATA_BUS) = 0;
  Z80_DATA_BUS = 0;
  DDR(Z80_ADDRL_BUS) = 0;
  Z80_ADDRL_BUS = 0;
  DDR(Z80_ADDRH_BUS) = 0;
  Z80_ADDRH_BUS = 0;

  pinMode(Z80_CLK_PIN, OUTPUT);
  
  // set pin mode to INPUT for outputs of Z80
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

  // set pin mode to OUTPUT and set pull-ups if necessary for inputs of Z80
#ifdef Z80_NMI_PIN
  pinMode(Z80_NMI_PIN, OUTPUT);
  //digitalWrite(Z80_NMI_PIN, HIGH);
#endif
#ifdef Z80_INT_PIN
  pinMode(Z80_INT_PIN, OUTPUT);
  //digitalWrite(Z80_INT_PIN);
#endif
  pinMode(Z80_BUSREQ_PIN,OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
  pinMode(Z80_WAIT_PIN,OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
  pinMode(Z80_RESET_PIN,OUTPUT);
  //digitalWrite(Z80_RESET_PIN, HIGH);

}

void z80_reset(unsigned long dur) {
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(dur);
  digitalWrite(Z80_RESET_PIN, HIGH);
}

