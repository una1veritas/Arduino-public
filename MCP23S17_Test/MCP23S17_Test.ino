#include <SPI.h>
#include <MCP23S17.h>

enum SPI_SLAVES {
  CS_23LC1024 = 10,
  CS_MCP23S08 = 9,
  CS_MCP23S17 = 8,
};

enum LED7SEG {
  E   = 1<<0, 
  D   = 1<<1, 
  C   = 1<<2, 
  DP  = 1<<3, 
  B   = 1<<4,
  A   = 1<<5,
  F   = 1<<6,
  G   = 1<<7, 
};

const byte ten_secs[6] = {
  A, 
  B, 
  C, 
  D, 
  E, 
  F, 
};

MCP23S17 ioxt(CS_MCP23S17, 1);

void setup() {
  Serial.begin(9600);
  Serial.println("Hello.");

  // ensure to disable all the SPI slave devices. 
  pinMode(CS_23LC1024, OUTPUT); digitalWrite(CS_23LC1024, HIGH);
  pinMode(CS_MCP23S08, OUTPUT); digitalWrite(CS_MCP23S08, HIGH);
  pinMode(CS_MCP23S17, OUTPUT); digitalWrite(CS_MCP23S17, HIGH);

  SPI.begin();
  Serial.println("SPI started.");
  ioxt.begin();
  ioxt.hw_address_enable();
  Serial.println("IO Expander started.");
  
  ioxt.set_outputA();
  ioxt.set_outputB();
  ioxt.gpioA(0xff);
}

void loop() {
  long sec = millis()/1000;
  uint8_t bits = ten_secs[(sec/10) % 6] | (sec & 1 ? 0 : G) | (sec & 1 ? DP : 0);
  ioxt.gpio16( bits^0x00ff );
  delay(100);
}

