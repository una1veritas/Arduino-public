#include <SPI.h>
#include "MCP23S1x.h"

MCP23S1x io(10, 7);
long cnt;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello.");
  pinMode(10, OUTPUT);
  SPI.begin();
  
  Serial.println("start.");
  io.begin();
  io.portAMode(io.IODIR_INPUT);
  io.portAMode(io.IODIR_INPUT);
  
  cnt = 0;
}

void loop() {
//  io.write(io.GPIOB, cnt++);
}

