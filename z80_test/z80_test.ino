#include "z80io.h"
#include "z80io_pindef.h"
#include "sram.h"
#include "sram_pindef.h"

void setup() {
  Serial.begin(19200);
  Serial.println("Hello.");

  sram_init();
  DATA_DIR = 0xff;
  DATA_OUT = 0x00;
  
  pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, HIGH);
  /*
  pinMode(Z80_INT_PIN, OUTPUT);
  digitalWrite(Z80_INT_PIN, HIGH);
  pinMode(Z80_NMI_PIN, OUTPUT);
  digitalWrite(Z80_NMI_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
  */
  // OCA1 as Z80 clock source
  init_Timer1(5,16000);

  digitalWrite(Z80_RESET_PIN, LOW);
  delay(500);
  digitalWrite(Z80_RESET_PIN, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
}
